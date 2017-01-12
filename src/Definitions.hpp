#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <Arduino.h>
#include <stdint.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>


/****************MACRO DEFINITIONS BEGIN******************/

//#define DEBUG //to be commented out when building the code for final release

/*  To be used by read_settings to identify when the module is
 *  being run for the first time hence determining whether to set the
 *  global settings to be used for the runtime by reading the
 *  the settings from the EEPROM or to use the default compiled
 *  settingssss
 */
#define SECOND_RUN char(0xF8)

#define TRUE 0x01 // 1 byte (8 bits) of data to be used in writing to EEPROM
#define FALSE 0x00  //1 byte (8 bits) of data to be used in writing to EEPROM

#define LED_1 4 //pin number on the board for POWER IN (from mains) display led
#define LED_2 5 //pin number on the board for POWER OUT (refrigerator on) display led
#define LED_3 6 //pin number on the board for SMS RECEIVED (under processing) display led

#define PWRKEY 9  //pin number on the board for GSM shield power control
#define RELAY_CONTROL 0 //pin number on the board for relay control
#define COMPARTMENT1 A0 //pin number on the board for analog read of temperature 1
//pin number on the board for analog read of the negative output of the first LM35 sensor
#define COMPARTMENT1_REF A1 
#define COMPARTMENT2 A2 //pin number on the board for analog read of temperature 2
//pin number on the board for analog read of the negative output of the second LM35 sensor
#define COMPARTMENT2_REF A3 

#define DEFAULT_AUTO_POWER TRUE //default setting for refrigerator powering
#define DEFAULT_AUTO_ACK TRUE //default setting for responding to commands with ACK/NACK
#define DEFAULT_PIN "3213" //default password for security purposes (Can be changed by the end user)

/*  Since the least minimum temperature that occurs in a
 *  refrigerator is that of the freezer at a temperature 
 *  of -18'c. We set our no limit on the operating 
 *  temperature of the refrigerator to -30'c. This guarantees
 *  us that in real time and practical usage that there will
 *  be no limit to the operating temperature of the 
 *  refrigerator
 */

#define NO_TEMP_LIMIT -40.0

/* Different temperature definitions for 
 */
#define MEAT -30.0
#define SEAFOOD -25.0
#define COOKED -18.0
#define ICECREAM -12.0
#define DAIRY 4.0
#define VEGFRUIT 4.0
#define WATER 4.0

#define POWER_ON digitalWrite(RELAY_CONTROL, HIGH);  \
                 digitalWrite(LED_2, HIGH);  \
                 gPower_On = TRUE;

#define POWER_OFF digitalWrite(RELAY_CONTROL, LOW);  \
                  digitalWrite(LED_2, LOW);  \
                  gPower_On = FALSE;

/****************MACRO DEFINITIONS END******************/



/****************GLOBAL VARIABLE DECLARATIONS BEGIN******************/

extern LiquidCrystal lcd_display; //global variable name for the liquid crystal display object
extern SoftwareSerial gsm_shield; //global variable name for the software serial object

extern uint8_t gPower_On;  //to be used to indicate when the refrigerator is powered ON and OFF
extern uint8_t gIs_Value_Changed; //indicates when a setting has been changed by the user

/*  Indicates which setting was changed by the user
 *  It ranges from 0 - 2
 *  0: auto power
 *  1: auto ack
 *  2: pin
 */
extern int8_t gValue_Changed;

/*  Holds the number from which a command SMS was
 *  received incase if an ACK/NACK is required to
 *  be sent back to the command issuer
 */
extern String gNumber;

//Holds the value of Auto powering of the refrigerator during runtime
extern uint8_t gAuto_Power;

//Holds the value of Auto Ack of the module during runtime
extern uint8_t gAuto_Ack;

/*  Holds the PIN of authorized users that are allowed 
 *  to issue commands to the module.
 *  It must be 4 characters
 */
extern String gPIN;

/*  Holds the value of the temperature limits for both
 *  compartments (freezer and refrigerator)
 */
extern float gCompartment1_Temp_Limit;
extern float gCompartment2_Temp_Limit;

/****************GLOBAL VARIABLE DECLARATIONS END******************/


/****************SUB ROUTINES DECLARATIONS BEGIN*******************/

//Initial setup routines
void toggle_shield_power();
void setup_environment();

//SMS handling routines
uint8_t incoming_sms();
String read_sms();
void send_sms(String&);

//Settings handling routines
void read_settings();
void write_settings();

//Central Control routines
void master_controller();
void temperature_power_control();
String parse_sms_to_command(String&, uint8_t&);

//Individual/Specific control routines
void power_on_refrigerator();
void power_off_refrigerator();
void auto_power_setting(String&);
void auto_ack_setting(String&);
void new_pin_setting(String&);
void temp_limit_setting(uint8_t, String&);
void refrigerator_status();

//Information display routine
void status_display(uint8_t);
float _temperature_read(uint8_t);


inline String temperature_read(uint8_t compartment)
{
  return String(_temperature_read(compartment), 1) + String(char(0xDF)) + "C";
}


inline float string_to_temp(uint8_t compartment, String& value)
{
  float temp = -50.0;
  if(value == "water")
    temp = WATER;
  else if(value == "icecream" && compartment == 1)
    temp = ICECREAM;  
  else if(value == "icecream" && compartment == 2)
    temp = WATER;
  else if(value == "meat" && compartment == 1)
    temp = MEAT;
  else if(value == "seafood" && compartment == 1)
    temp = SEAFOOD;
  else if(value == "cooked" && compartment == 1)
    temp = COOKED;
  else if(value == "dairy" && compartment == 2)
    temp = DAIRY;
  else if(value == "vegfruit" && compartment == 2)
    temp = VEGFRUIT;
  return temp;
}

/*  Acknowledgement routines.
 *  This routines came in as a result 
 *  of code refactoring which was caused
 *  by the limited size of the dynamic 
 *  memory (2K). In other to avoid code
 *  repetitions that contains String
 *  data variables which consume a lot of
 *  dynamic memory space, we use single 
 *  routines to group together operations 
 *  that have similar String contents. 
 *  This way we save more space on the 
 *  dynamic memory for local routine 
 *  computations
 */
void acknowledge_command(String);
void acknowledge_same_setting(String);
void acknowledge_same_state(String);
void unacknowledge_command(String);

/****************SUB ROUTINES DECLARATIONS END*******************/

#endif  //DEFINITIONS_H
