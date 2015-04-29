/************************************************************************
  This is an example program which uses the SHT21 Temperature & Humidity 
  Sensor library for use on the RaspberryPi Single Board Computer

  Author: Ondrej Wisniewski
  
  Build command (make sure to have shtlib built and installed):
  gcc -o shtsensor shtsensor.c -lsht
  
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "sht21.h"

int main(int argc, char* argv[])
{
   int16_t temperature;
   uint16_t humidity;
   uint8_t err;

   /* Init the library */
   SHT21_Init(24,25);
   
   /* Read temperature and humidity from sensor */
   err = SHT21_Read(&temperature, &humidity);
   
   if (SHT21_Cleanup() != 0)
   {
      printf("ERROR during SHT cleanup\n");
      return -1;
   }
   
   if (err == 0 )
   {
      printf("T=%.1fC\tH=%.1f%%\n", temperature/10.0, humidity/10.0);
   }
   else
   {
      printf("ERROR 0x%X reading sensor\n", err);
   }
   
   return 0;
}
