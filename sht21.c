//------------------------------------------------------------------------------
//
// Filename:    sht21.c
// Description: This file is part of the libsht library. 
//              Implements the specific functions to read the Sensirion SHT21
//              temperature and humidity sensor using the simulated I2C protocol
//
// Open Source Licensing 
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//              
// Author:      Martin Steppuhn, Ondrej Wisniewski
// History:     14.09.2012 (MS) Initial version "Quick and Dirty" 
//              23.04.2015 (OW) Added SHT21_Init()
//              24.04.2015 (OW) Changed humidity calculation, code cleanup
//              27.04.2015 (OW) Added SHT21_Cleanup()
//------------------------------------------------------------------------------

/**** Includes ****************************************************************/

#include <stdint.h>
#include <unistd.h>
#include "bcm2835.h"
#include "i2c.h"

/**** Preprocessing directives (#define) **************************************/

/**** Type definitions (typedef) **********************************************/

/**** Global constants ********************************************************/

/**** Global variables ********************************************************/

/**** Local constants  ********************************************************/

// SHT21 I2C address
#define I2C_ADDR      0x40

// Sensor commands
#define CMD_TMP_HLD   0xE3
#define CMD_HUM_HLD   0xE5
#define CMD_TMP_NOHLD 0xF3
#define CMD_HUM_NOHLD 0xF5
#define CMD_WR_REG    0xE6
#define CMD_RD_REG    0xE7
#define CMD_SOFT_RST  0xFE


/**** Local variables *********************************************************/

static uint8_t lib_initialised=0;


/**** Local function prototypes ***********************************************/

static uint8_t SHT21_CalcCrc(uint8_t *data,uint8_t nbrOfBytes);


//------------------------------------------------------------------------------
// Name:      SHT21_Init
// Function:  Initialise the SHT library
//            
// Parameter: uint8_t scl : pin used for clock line
//            uint8_t sda : pin used for data line
//
// Return:     0: SUCCESS
//            >0: ERROR
//------------------------------------------------------------------------------
uint8_t SHT21_Init(uint8_t scl,uint8_t sda)
{
   if (!lib_initialised)
   {
      if (bcm2835_init() == 0)
      {
         return 1;
      }
      
      lib_initialised = 1;
   }
   
   SI2C_SetPort(scl, sda);
   return 0;
}

//------------------------------------------------------------------------------
// Name:      SHT21_Cleanup
// Function:  Cleanup resources used by SHT library
//            
// Parameter: None
//            
// Return:     0: SUCCESS
//            >0: ERROR
//------------------------------------------------------------------------------
uint8_t SHT21_Cleanup(void)
{
   if (lib_initialised)
   {
      if (bcm2835_close() == 0)
      {
         return 1;
      }
      lib_initialised = 0;
   }
   
   return 0;
}

//------------------------------------------------------------------------------
// Name:      SHT21_Read
// Function:  Read temperature and humidity from SHT21 sensor
//            
// Parameter: int16_t *temp      : temperature (in 10th C)
//            uint16_t *humidity : rel. humidity (in 10th %)
//
// Return:     0: SUCCESS
//            >0: ERROR
//------------------------------------------------------------------------------
uint8_t SHT21_Read(int16_t *temp, uint16_t *humidity)
{
   uint8_t error;
   uint8_t d[3];
   uint8_t timeout;
   uint32_t val;
   
   error = 0;
   
   //=== Software reset =============================================
   
   SI2C_Start();
   error |= SI2C_SendByte((I2C_ADDR << 1) + 0);	// Addr + WR
   error |=SI2C_SendByte(CMD_SOFT_RST);		// Soft reset
   SI2C_Stop();
   
   usleep(15000);
   
   //=== User register ======================================================== 
   
   SI2C_Start();
   error |= SI2C_SendByte((I2C_ADDR << 1) + 0);	// Addr + WR
   error |= SI2C_SendByte(CMD_RD_REG);		// Read user register
   SI2C_Start();
   error |= SI2C_SendByte((I2C_ADDR << 1) + 1);	// Addr + RD
   d[0] = SI2C_ReadByte(1);			
   d[1] = SI2C_ReadByte(0);
   SI2C_Stop();  
   
   if(d[0] == 0) 
   {
      error |= 0x02;
   }
   else if(d[1] == SHT21_CalcCrc(d,1))
   {
      SI2C_Start();
      error |= SI2C_SendByte((I2C_ADDR << 1) + 0);	// Addr + WR
      error |= SI2C_SendByte(CMD_WR_REG);		// User register
      error |= SI2C_SendByte(d[0]);			// Value 
      SI2C_Stop();
   }
   else
   {
      error |= 0x04;
   }
   
   //=== Temperature ===========================================================  	
   
   SI2C_Start();
   error |= SI2C_SendByte((I2C_ADDR << 1) + 0);
   error |= SI2C_SendByte(CMD_TMP_HLD);
   SI2C_Start();
   error |= SI2C_SendByte((I2C_ADDR << 1) + 1);
   SI2C_SetSclState(1);
   
   timeout = 100;
   while(SI2C_GetSclState()  == 0 && timeout)
   {
      usleep(1000);
      timeout--;
   }
   if(timeout == 0) error |= 0x08;
   
   d[0] = SI2C_ReadByte(1);
   d[1] = SI2C_ReadByte(1);
   d[2] = SI2C_ReadByte(0);
   SI2C_Stop();
   
   if(d[2] == SHT21_CalcCrc(d,2))
   {
      val = d[0];
      val <<= 8;
      val += d[1];
      val &= 0xFFFC;
      
      //	T = -46,85 + 175,72 * St/65535      da 1/10K -->  * 10
      //	T = -468,5 + 1757,2 * St/65535		verinfachen
      //	T = -468,5 + St / 37,2956..		damit Konstante ganzzahlig wird mit 2 erweitern
      //        T = -937 + 2*St / 37,2956..		Bruch für Division mit 256 erweitern  
      //	T = (-937 +  (St * 512) / (37,2956.. * 256)  )  / 2
      //	T = (((St * 512) / 9548) - 937) / 2
      
      *temp = ((val * 512) / 9548);
      *temp = ((*temp) - 937) / 2;       
   }
   else
   {
      error |= 0x10;
   }
   
   //=== Humidity ==============================================================
   
   SI2C_Start();
   error |= SI2C_SendByte((I2C_ADDR << 1) + 0);
   error |= SI2C_SendByte(CMD_HUM_HLD);
   SI2C_Start();
   error |= SI2C_SendByte((I2C_ADDR << 1) + 1);
   SI2C_SetSclState(1);
   timeout = 100;
   while(SI2C_GetSclState() == 0 && timeout)
   {
      usleep(1000);
      timeout--;
   }
   if(timeout == 0) error |= 0x20;
   
   d[0] = SI2C_ReadByte(1);
   d[1] = SI2C_ReadByte(1);
   d[2] = SI2C_ReadByte(0);
   SI2C_Stop();
   
   
   if(d[2] == SHT21_CalcCrc(d,2))
   {
      val = d[0];
      val <<= 8;
      val += d[1];
      val &= 0xFFFC;
      
      //   T = -6 + 125* Srh/65535      
      //   T = -6 + Srh / 524,28
      //   T = -6 + (Srh * 256) / 134215      |  *256	 wegen Numerik erweitern
      //val = ((val * 256) / 134215) - 6;
      
      val = (uint32_t)(10*((val * 256) / 134217.73 - 6));
      *humidity = (uint16_t)val;
   }
   else
   {
      error |= 0x40;
   }
   return(error);
}

//------------------------------------------------------------------------------
// Name:      SHT21_CalcCrc
// Function:  
//            
// Parameter: uint8_t *data      : pointer to data buffer
//            uint8_t nbrOfBytes : number of bytes
// Return:    
//------------------------------------------------------------------------------
static uint8_t SHT21_CalcCrc(uint8_t *data,uint8_t nbrOfBytes)
{
   // CRC
   //const u16t POLYNOMIAL = 0x131; //P(x)=x^8+x^5+x^4+1 = 100110001
   
   uint8_t byteCtr,bit,crc;
   
   crc = 0;
   
   //calculates 8-Bit checksum with given polynomial
   for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
   { 
      crc ^= (data[byteCtr]);
      for (bit = 8; bit > 0; --bit)
      {
         if (crc & 0x80) crc = (crc << 1) ^ 0x131;
         else 		crc = (crc << 1);
      }
   }
   return(crc);
}
