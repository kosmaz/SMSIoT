#include "Definitions.hpp"
#include <EEPROM.h>


void read_settings()
{
  
  /*  Read the first byte of the EEPROM to check
   *  whether the value 0xF8 has been written 
   *  before. If it has been written before
   *  then we can read the settings value from
   *  EEPROM and if it hasn't been written before
   *  then we use the compiled default settings.
   */
  char is_first_run = '\0';
  is_first_run = EEPROM.read(0); 

  if(is_first_run == SECOND_RUN)
  {
#ifdef DEBUG
    Serial.println(F("READ SETTINGS: SECOND RUN"));
#endif
    /*  Read from the last successful write locations
     *  to ensure that the value isn't corrupted due 
     *  to sudden power outage during the write process.
     *  The memory mapping is:
     *  auto_power read from  : 1
     *  auto_power            : 2        represented by 0x01
     *  auto_power            : 3        represented by 0x02
     *  
     *  auto_ack read from    : 4
     *  auto_ack              : 5        represented by 0x01
     *  auto_ack              : 6        represented by 0x02
     *  
     *  pin read from         : 7
     *  pin                   : 8 - 11   represented by 0x01
     *  pin                   : 12 - 15  represented by 0x02
     *  
     *  temp1_limit read from :16
     *  temp1_limit           :17-20     represented by 0x01
     *  temp1_limit           :21-24     represented by 0x02
     *  
     *  temp2_limit read from :25
     *  temp2_limit           :26-29     represented by 0x01
     *  temp2_limit           :30-33     represented by 0x02
     */
    char auto_power_location = EEPROM.read(1);
    if(auto_power_location == 0x01)
      gAuto_Power = EEPROM.read(2);
    else if(auto_power_location == 0x02)
      gAuto_Power = EEPROM.read(3);

    char auto_ack_location = EEPROM.read(4);
    if(auto_ack_location == 0x01)
      gAuto_Ack = EEPROM.read(5);
    else if(auto_ack_location == 0x02)
      gAuto_Ack = EEPROM.read(6);

    char pin_location = EEPROM.read(7);
    if(pin_location == 0x01)
    {
      gPIN = "";
      char c = EEPROM.read(8);
      gPIN += c;
      c = EEPROM.read(9), gPIN += c;
      c = EEPROM.read(10), gPIN += c;
      c = EEPROM.read(11), gPIN += c;       
    }
    else if(pin_location == 0x02)
    {
      gPIN = "";
      char c = EEPROM.read(12);
      gPIN += c;
      c = EEPROM.read(13), gPIN += c;
      c = EEPROM.read(14), gPIN += c;
      c = EEPROM.read(15), gPIN += c; 
    }

    char temp1_limit_location = EEPROM.read(16);
    if(temp1_limit_location == 0x01)
      EEPROM.get(17, gCompartment1_Temp_Limit);
    else if(temp1_limit_location == 0x02)
      EEPROM.get(21, gCompartment1_Temp_Limit);
      
    char temp2_limit_location = EEPROM.read(25);
    if(temp2_limit_location == 0x01)
      EEPROM.get(26, gCompartment2_Temp_Limit);
    else if(temp2_limit_location == 0x02)
      EEPROM.get(30, gCompartment2_Temp_Limit);
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("READ SETTINGS: FIRST RUN"));
#endif
    /*  Try to write to EEPROM for the first time
     *  so that successive settings will be read  
     *  from EEPROM instead of using the compiled
     *  defualt settings
     */
    gIs_Value_Changed = TRUE;
    for(int i = 0; i < 5; ++i)
    {
      gValue_Changed = i;
      write_settings();
    }
    gIs_Value_Changed = FALSE;
    gValue_Changed = -1;
    EEPROM.write(0, SECOND_RUN);
    delay(4); //wait for 4ms for write operation to complete
  }
  
  return;
}


void write_settings()
{
  /*  Write to different memory locations on each
   *  successive write operation to ensure that the 
   *  value isn't corrupted due to sudden power 
   *  outage during the write process. This method
   *  though highly redundant ensures that we always 
   *  have a valid location to read in an uncorrupted
   *  value from during environment setup.
   *  The memory mapping is:
   *  auto_power read from  : 1
   *  auto_power            : 2        represented by 0x01
   *  auto_power            : 3        represented by 0x02
   *  
   *  auto_ack read from    : 4
   *  auto_ack              : 5        represented by 0x01
   *  auto_ack              : 6        represented by 0x02
   *  
   *  pin read from         : 7
   *  pin                   : 8 - 11   represented by 0x01
   *  pin                   : 12 - 15  represented by 0x02
   *  
   *  temp1_limit read from :16
   *  temp1_limit           :17-20     represented by 0x01
   *  temp1_limit           :21-24     represented by 0x02
   *  
   *  temp2_limit read from :25
   *  temp2_limit           :26-29     represented by 0x01
   *  temp2_limit           :30-33     represented by 0x02
   */  
  if(gIs_Value_Changed)
  {
    switch(gValue_Changed)
    {
      case 0: //write auto_power value to memory
      {
        char auto_power_location = EEPROM.read(1);
        if(auto_power_location == 0x01)
        {
          /*  Write to second memory location if the
           *  last successful write was at first memory
           *  location.          
           */
          EEPROM.write(3, gAuto_Power);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(1, 0x02);
          delay(4); //wait for 4ms for write operation to complete
        }
        else
        {
          /*  Write to first memory location if the
           *  last successful write was at second memory
           *  location or the value of the address location
           *  1 is corrupted due to a sudden power outage 
           *  during the last write operation.
           */
          EEPROM.write(2, gAuto_Power);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(1, 0x01);
          delay(4); //wait for 4ms for write operation to complete
        }
        break;
      }

      case 1: //write auto_ack value to memory
      {    
        char auto_ack_location = EEPROM.read(4);
        if(auto_ack_location == 0x01)
        {
          /*  Write to second memory location if the
           *  last successful write was at first memory
           *  location.          
           */
          EEPROM.write(6, gAuto_Ack);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(4, 0x02);
          delay(4); //wait for 4ms for write operation to complete
        }
        else
        {
          /*  Write to first memory location if the
           *  last successful write was at second memory
           *  location or the value of the address location
           *  4 is corrupted due to a sudden power outage 
           *  during the last write operation.
           */
          EEPROM.write(5, gAuto_Ack);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(4, 0x01);
          delay(4); //wait for 4ms for write operation to complete
        }
        break;
      }

      case 2:  //write pin value to memory
      {
        char pin_location = EEPROM.read(7);
        if(pin_location == 0x01)
        {
          /*  Write to second memory location if the
           *  last successful write was at first memory
           *  location.          
           */
          EEPROM.write(12, gPIN[0]);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(13, gPIN[1]);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(14, gPIN[2]);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(15, gPIN[3]);
          delay(4); //wait for 4ms for write operation to complete  
          EEPROM.write(7, 0x02);
          delay(4); //wait for 4ms for write operation to complete
          
        }
        else
        {
          /*  Write to first memory location if the
           *  last successful write was at second memory
           *  location or the value of the address location
           *  7 is corrupted due to a sudden power outage 
           *  during the last write operation.
           */
          EEPROM.write(8, gPIN[0]);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(9, gPIN[1]);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(10, gPIN[2]);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(11, gPIN[3]);
          delay(4); //wait for 4ms for write operation to complete 
          EEPROM.write(7, 0x01);
          delay(4); //wait for 4ms for write operation to complete 
        }
        break;
      }

      case 3: //write temp1_limit value to memory 
      {
        char temp1_limit_location = EEPROM.read(16); 
        if(temp1_limit_location == 0x01)
        {
          /*  Write to second memory location if the
           *  last successful write was at first memory
           *  location.          
           */
          EEPROM.put(21, gCompartment1_Temp_Limit);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(16, 0x02);
          delay(4); //wait for 4ms for write operation to complete
        }
        else
        {
          /*  Write to first memory location if the
           *  last successful write was at second memory
           *  location or the value of the address location
           *  16 is corrupted due to a sudden power outage 
           *  during the last write operation.
           */
          EEPROM.put(17, gCompartment1_Temp_Limit);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(16, 0x01);
          delay(4); //wait for 4ms for write operation to complete
        }
        break;
      }

      case 4:  //write temp2_limit value to memory
      {
        char temp2_limit_location = EEPROM.read(25);
        if(temp2_limit_location == 0x01)
        {
          /*  Write to second memory location if the
           *  last successful write was at first memory
           *  location.          
           */
          EEPROM.put(30, gCompartment2_Temp_Limit);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(25, 0x02);
          delay(4); //wait for 4ms for write operation to complete
        }
        else
        {
          /*  Write to first memory location if the
           *  last successful write was at second memory
           *  location or the value of the address location
           *  25 is corrupted due to a sudden power outage 
           *  during the last write operation.
           */
          EEPROM.put(26, gCompartment2_Temp_Limit);
          delay(4); //wait for 4ms for write operation to complete
          EEPROM.write(25, 0x01);
          delay(4); //wait for 4ms for write operation to complete
        }
        break;
      }

      default: break;
    }
  }
  return;
}


