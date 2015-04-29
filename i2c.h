//--------------------------------------------------------------------------------------------------
//
// Filename:    i2c.h
// Description: I2C Software Implementierung
//              
// Author:      Martin Steppuhn
// History:     28.08.2012 Initial version
//              31.10.2012 Flexible pin connection
//--------------------------------------------------------------------------------------------------

#ifndef I2C_H
#define I2C_H

//=== Includes =====================================================================================	

//=== Preprocessing directives (#define) ===========================================================

//=== Type definitions (typedef) ===================================================================

//=== Global constants (extern) ====================================================================

//=== Global variables (extern) ====================================================================

//=== Global function prototypes ===================================================================

void  SI2C_SetPort(uint8_t Scl,uint8_t Sda);
void  SI2C_Start(void);
void  SI2C_Stop(void);
uint8_t SI2C_SendByte(uint8_t Data);
uint8_t SI2C_ReadByte(uint8_t Ack);
void  SI2C_SetSclState(uint8_t State);
uint8_t SI2C_GetSclState(void);

#endif
