//------------------------------------------------------------------------------
//
// Filename:    sht21.h
// Description: This file is part of the libsht library. 
//              Declares the specific functions to read the Sensirion SHT21
//              temperature and humidity sensor using the simulated I2C protocol
//              
// Author:      Martin Steppuhn, Ondrej Wisniewski
// History:     26.11.2011 (MS) Initial version
//              23.04.2015 (OW) Added SHT21_Init()
//              24.04.2015 (OW) Code cleanup
//              27.04.2015 (OW) Added SHT21_Cleanup()
//------------------------------------------------------------------------------

#ifndef SHT21_H
#define SHT21_H

/**** Includes ****************************************************************/

/**** Preprocessing directives (#define) **************************************/

/**** Type definitions (typedef) **********************************************/

/**** Global constants (extern) ***********************************************/

/**** Global variables (extern) ***********************************************/

/**** Global function prototypes **********************************************/

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
uint8_t SHT21_Init(uint8_t scl,uint8_t sda);

//------------------------------------------------------------------------------
// Name:      SHT21_Cleanup
// Function:  Cleanup resources used by SHT library
//            
// Parameter: None
//            
// Return:     0: SUCCESS
//            >0: ERROR
//------------------------------------------------------------------------------
uint8_t SHT21_Cleanup(void);

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
uint8_t SHT21_Read(int16_t *temp,uint16_t *humidity);

#endif
