#include "Definitions.hpp"


void power_on_refrigerator()
{
  //power ON the refrigerator if it is powered OFF and the setting isn't left in auto powering
  String buff = F("poweron");
  if(!gPower_On && !gAuto_Power)
  {
    POWER_ON

    //Acknowledge the command if Auto acknowledgement is allowed
    if(gAuto_Ack)
      acknowledge_command(buff);
  }
  else if(gPower_On && gAuto_Ack && !gAuto_Power)
    acknowledge_same_state(F("ON"));
  else if(gAuto_Ack)
    unacknowledge_command(buff);
  return;
}


void power_off_refrigerator()
{
  //power OFF the refrigerator if it is powered ON and the setting isn't left in auto powering
  String buff = F("poweroff");
  if(gPower_On && !gAuto_Power)
  {
    POWER_OFF

    //Acknowledge the command if Auto acknowledgement is allowed
    if(gAuto_Ack)
      acknowledge_command(buff);
  }
  else if(!gPower_On && gAuto_Ack && !gAuto_Power)
    acknowledge_same_state(F("OFF"));
  else if(gAuto_Ack)
    unacknowledge_command(buff);
  return;
}


void auto_power_setting(String& value)
{
  /*  We need to make sure that the user is not trying to
   *  change the value of gAuto_Power with the same value 
   *  which it already holds in memory. 
   *  By confirming that the value specified by the user is
   *  different from the value stored in memory we can mitigate
   *  the number of writes performed on the memory (EEPROM).
   *  This is to improve the life cycle of the EEPROM since it
   *  has a limited number of write cycles (100K). 
   */
  String buff = F("autopower");
  if((value == "1" && gAuto_Power == FALSE) || (value == "0" && gAuto_Power == TRUE))
  {
    //flip the content of gAuto_Power between TRUE and FALSE
    gAuto_Power ^= TRUE;  
    gIs_Value_Changed = TRUE;
            
    //identity of gAuto_Power in memory mapping for the settings
    gValue_Changed = 0; 
    write_settings();
    gIs_Value_Changed = FALSE;
    gValue_Changed = -1;

    //Acknowledge the command if Auto acknowledgement is allowed
    if(gAuto_Ack)
      acknowledge_command(buff);
  }
  else if(((value == "1" && gAuto_Power == TRUE) || (value == "0" && gAuto_Power == FALSE)) && gAuto_Ack)
    acknowledge_same_setting(buff);
  else if(gAuto_Ack)
    unacknowledge_command(buff);
  return;
}


void auto_ack_setting(String& value)
{
  /*  We need to make sure that the user is not trying to
   *  change the value of gAuto_Ack with the same value 
   *  which it already holds in memory. 
   *  By confirming that the value specified by the user is
   *  different from the value stored in memory we can mitigate
   *  the number of writes performed on the memory (EEPROM).
   *  This is to improve the life cycle of the EEPROM since it
   *  has a limited number of write cycles (100K). 
   */
  String buff = F("autoack");
  if((value == "1" && gAuto_Ack == FALSE) || (value == "0" && gAuto_Ack == TRUE))
  {
    //flip the content of gAuto_Ack between TRUE and FALSE
    gAuto_Ack ^= TRUE;  
    gIs_Value_Changed = TRUE;
            
    //identity of gAuto_Ack in memory mapping for the settings
    gValue_Changed = 1; 
    write_settings();
    gIs_Value_Changed = FALSE;
    gValue_Changed = -1;

    //Acknowledge the command if Auto acknowledgement is allowed
    if(gAuto_Ack)
      acknowledge_command(buff);
  }
  else if(((value == "1" && gAuto_Ack == TRUE) || (value == "0" && gAuto_Ack == FALSE)) && gAuto_Ack)
    acknowledge_same_setting(buff);
  else if(gAuto_Ack)
    unacknowledge_command(buff);
  return;
}


void new_pin_setting(String& value)
{
  /*  To ensure that the user doesn't accidentally
   *  enter a random pin that he/she doesn't remember
   */
  String confirm_pin = "";
  String buff = F("newpin");

  /*  Split the value of the newpin command into two
   *  strings. One to hold the value of the new pin
   *  and one to hold the value of the confirmation
   *  pin.
   */
  for(unsigned i = 0; i < value.length(); ++i)
    if(value[i] == ',')
    {
      /*  Ensure that we are not trying to access an area
       *  ouside of the sms string in memory to avoid the
       *  program crashing unexpectedly.
       */
      if(i != value.length() - 1)
        confirm_pin = value.substring(i + 1);
      value = value.substring(0, i);
      break;
    }
    
  if(value == confirm_pin && value.length() == 4)
  {
    /*  We need to make sure that the user is not trying to
     *  change the value of gPIN with the same value 
     *  which it already holds in memory. 
     *  By confirming that the value specified by the user is
     *  different from the value stored in memory we can mitigate
     *  the number of writes performed on the memory (EEPROM).
     *  This is to improve the life cycle of the EEPROM since it
     *  has a limited number of write cycles (100K). 
     */
    if(value != gPIN)
    {
      gPIN = value;
      gIs_Value_Changed = TRUE;
            
      //identity of gPIN in memory mapping for the settings
      gValue_Changed = 2; 
      write_settings();
      gIs_Value_Changed = FALSE;
      gValue_Changed = -1;
  
      //Acknowledge the command if Auto acknowledgement is allowed
      if(gAuto_Ack)
        acknowledge_command(buff);
    }
    else if((value == gPIN) && gAuto_Ack)
      acknowledge_same_setting(buff);
  }
  else if(gAuto_Ack)
    unacknowledge_command(buff);
  return;
}


void temp_limit_setting(uint8_t compartment, String& value)
{
  /*  This routine handles saving of the temperature
   *  limits for both the freezer and the refrigertor to 
   *  EEPROM. It handles situations such as when the user 
   *  is trying to set the same temperature limit value of 
   *  any of the compartment. It also handles situations
   *  such as when the user tries to input a value of a
   *  refrigerator item to a compartment which belongs to 
   *  a different compartment.
   */
  if(compartment == 1)
  {
    //  Compartment 1 = FREEZER
    String buff = F("settemp1");
    float temp = string_to_temp(1, value);

    /*  If the given item isn't meant to be
     *  in the freezer, we should not acknowledge 
     *  the command and as well tell the user
     *  if auto_ack is enabled.
     */
    if(temp == -50.0 && gAuto_Ack)
      unacknowledge_command(buff);
    else if(temp == gCompartment1_Temp_Limit && gAuto_Ack)
      acknowledge_same_setting(buff);
    else
    {
      gCompartment1_Temp_Limit = temp;
      
      //Identity of gCompartment1_Temp_Limit in memory mapping for the settings
      gValue_Changed = 3;
      gIs_Value_Changed = TRUE;
      write_settings();
      gIs_Value_Changed = FALSE;
      gValue_Changed = -1;

      //Acknowledge the command if Auto acknowledgement is enabled
       if(gAuto_Ack)
        acknowledge_command(buff);
    }
  }
  else
  {
    //  Compartment 2 = REFRIGERATOR
    String buff = F("settemp2");
    float temp = string_to_temp(2, value);

    /*  If the given item isn't meant to be
     *  in the refrigerator, we should not 
     *  acknowledge the command and as well 
     *  tell the user if auto_ack is enabled.s
     */
    if(temp == -50.0 && gAuto_Ack)
      unacknowledge_command(buff);
    else if(temp == gCompartment2_Temp_Limit && gAuto_Ack)
      acknowledge_same_setting(buff);
    else
    {
      gCompartment2_Temp_Limit = temp;
      
      //Identity of gCompartment1_Temp_Limit in memory mapping for the settings
      gValue_Changed = 4;
      gIs_Value_Changed = TRUE;
      write_settings();
      gIs_Value_Changed = FALSE;
      gValue_Changed = -1;

      //Acknowledge the command if Auto acknowledgement is enabled
       if(gAuto_Ack)
        acknowledge_command(buff);
    }
  }
  return;
}


void refrigerator_status()
{
  /*  Report the status of the refrigerator to
   *  the user starting with the temperatures
   *  and then followed by the power status of
   *  the refrigerator 
   *  Compartment 1 Temperature     SENSOR 1 READ
   *  Compartment 2 Temperature     SENSOR 2 READ
   *  Powered on                    YES/NO
   */
  String temp1 = temperature_read(1);
  temp1.replace(char(0xDF), '\'');
  String temp2 = temperature_read(2);
  temp2.replace(char(0xDF), '\'');
  String msg = "Compartment 1 Temp: " + temp1 + "\r\n"; 
  msg += "Compartment 2 Temp: " + temp2 + "\r\n";
  if(gPower_On)
    msg += F("Powered On: YES\r\n");
  else
    msg += F("Powered On: NO\r\n");
            
  send_sms(msg);
  return;
}


