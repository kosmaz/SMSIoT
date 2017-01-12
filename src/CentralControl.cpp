#include "Definitions.hpp"


void master_controller()
{
  /*  Oversees the control of every aspect and 
   *  function of the module in real time. 
   *  It acts as an autonomous agent 
   *  (Reflex agent)
   */
  
  status_display(6);  //Display Information 6 on LCD

  /*  Hand over decision making to another routine 
   *  which has the proper algorithms and data to
   *  make autonomous decisions on how to leverage
   *  the available power to control the internal 
   *  temperature of the refrigerator compartments
   */
  temperature_power_control();
   
  if(incoming_sms())
  {    
    String sms = read_sms();

    /*  If the sms length is 0 then the read_sms routine
     *  doesn't want us to parse the sms for some reasons
     *  best know to it so it will be best in our interest 
     *  to abide by its judgement
     */
    if(sms.length() == 0)
      return;
      
    /*  send a HIGH to led 3 to turn it ON which indicates that an SMS has
     *  been received by the GSM shield and is currently been processed by
     *  the microcontroller for valid commands
     */
    digitalWrite(LED_3, HIGH);
    delay(1500);

    /*  Indicates whether the returned command_value is the 
     *  last in the body of the command list  
     */
    uint8_t is_last_command = FALSE; 

    /*  Retrieve the user's authorization pin first and check
     *  whether it is valid before going ahead to process the
     *  body of the command list.
     */
    if(parse_sms_to_command(sms, is_last_command) == gPIN)
      while(!is_last_command)
      {
        String value = "";
        String command = parse_sms_to_command(sms, is_last_command);

        /*  For commands that contains value, this helps to split
         *  the command string into its seperate command and value
         *  strings 
         *  The mapping of commands is as show below:
         *  poweron                                          HAS NO VALUE
         *  poweroff                                         HAS NO VALUE
         *  autopower=value   (1/0)                          HAS VALUE
         *  autoack=value     (1/0)                          HAS VALUE
         *  newpin=value      (newpin,confirm new pin)       HAS VALUE
         *  settemp1=value    (string)                       HAS VALUE 
         *  settemp2=value    (string)                       HAS VALUE
         *  status                                           HAS NO VALUE
         */
        for(unsigned i = 0; i < command.length(); ++i)
          if(command[i] == '=')
          {
            /*  Ensure that we are not trying to access an area
             *  ouside of the sms string in memory to avoid the
             *  program crashing unexpectedly.
             */
            if(i != command.length() - 1)
              value = command.substring(i + 1);
            command = command.substring(0, i);
            break;
          }

        if(command == F("poweron"))
          power_on_refrigerator();
        else if(command == F("poweroff"))
          power_off_refrigerator();
        else if(command == F("autopower"))
          auto_power_setting(value);
        else if(command == F("autoack"))
          auto_ack_setting(value);
        else if(command == F("newpin"))
          new_pin_setting(value);
        else if(command == F("settemp1"))
          temp_limit_setting(1, value);
        else if(command == F("settemp2"))
          temp_limit_setting(2, value);
        else if(command == F("status"))
          refrigerator_status();
        else if(gAuto_Ack)
          unacknowledge_command(command);
      }
    else
    {
      String msg = F("UNAUTHORIZED USER ACCESS!!!");
      send_sms(msg);
    }
    
    /*  send a LOW to led 3 to turn it OFF which indicates that the
     *  microcontroller is done processing the SMS received from the
     *  GSM shield for valid commands and that all commands are presently
     *  already in effect
     */
    digitalWrite(LED_3, LOW);
  }  
  return;
}


void temperature_power_control()
{
  /*  This routine handles the autonomous control of the
   *  powering ON and OFF of the refrigerator when auto power
   *  is enabled by the end user. This routine uses the temperature
   *  reading of the freezer (Compartment 1) to make its decisions. 
   *  This is as a result of the freezer housing delicate food items 
   *  that requires constant freezing below 0 degree celsius unlike
   *  the refrigerator that has minimum temperature for its content 
   *  to be just 4 degree celsius. A difference value of 3 degree
   *  celsius measured by adding or subtracting the measured temperture
   *  reading from the sensor in the freezer to or from the temperature
   *  limit for the freezer set by the end user is used in deciding when
   *  to power ON or OFF the refrigerator. Before the refrigerator can
   *  be powered OFF the temperature of the fridge compartment must be
   *  less than or equal to 10 degrees celcius
   */
  if(gAuto_Power)
  {
    //auto power must be enabled for this routine to work
    
    float temp1 = _temperature_read(1);  //temperture reading of the freezer compartment
    float temp2 = _temperature_read(2); //temperature reading of the fridge compartment
    int8_t temp_difference = 0;
    temp_difference = gCompartment1_Temp_Limit - temp1;

    /*  power ON or OFF the refrigerator when the temp_difference 
     *  becomes equal to or exceeds 3 degree celcius
     */
    if(temp_difference >= 3  && temp2 <= 10.0 && gPower_On)
    {
      /*  Only power OFF the refrigerator when the temperature
       *  of the fridge compartment is less than or equal to
       *  10 degrees celcius
       */
      POWER_OFF
    }
    else if(temp_difference <= -3 && !gPower_On)
    {
      POWER_ON
    }
  }
  return;
}


String parse_sms_to_command(String& sms, uint8_t& is_last_command)
{
  String command = "";

  /*  For each time this routine is called it returns a 
   *  segment of the full sms string and removes the returned
   *  segment from the sms string. It then indicates whether  
   *  the returned segment is the the last segment of the 
   *  sms string.
   *  This way it can be called in a while loop until the
   *  last segment is reached
   */
  for(unsigned i = 0; i < sms.length(); ++i)
  {
    /*  split out a pin/command segment from the sms string
     *  and remove this segment from the sms string 
     */
    if(sms[i] == ';')
    {
      command = sms.substring(0, i);

      /*  Ensure that we are not trying to access an area
       *  ouside of the sms string in memory to avoid the
       *  program crashing unexpectedly.
       */
      if(i != sms.length() - 1)
        /* Truncate the main sms body removing
         * the already extracted command leaving behind
         * the ones yet to be extracted
         */
        sms = sms.substring(i + 1);
      else
        /*  Indicate that the last segment of the sms string
         *  has been reached
         */
        is_last_command = TRUE;
          
      break;
    }
    else if(i == sms.length() - 1)
    /*  This block is used to handle situations where a user
     *  forgets to use the command terminating character ';'
     *  in all the commands supplied. We need to check to see
     *  when we've reached the end of the string there by setting
     *  the is_last_command to true so as to avoid infinite looping
     *  in the master_controller routine.
     */
      is_last_command = TRUE;
  }
    
#ifdef DEBUG
  Serial.println("PARSE SMS TO COMMAND: " + command);
#endif
    
  return command;
}


