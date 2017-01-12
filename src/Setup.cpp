#include "Definitions.hpp"


void toggle_shield_power()
{
  status_display(1);  //Display Information 1 on LCD
  
  //Turn ON GSM shield
  digitalWrite(PWRKEY, LOW);
  delay(1000);
  digitalWrite(PWRKEY, HIGH);
  delay(2000);
  digitalWrite(PWRKEY, LOW);

#ifdef DEBUG
  Serial.begin(19200);  //to be used for debugging the code
  while(!Serial); //wait for serial monitor to be paired with the dev system
  Serial.println(F("GSM Shield started"));
#endif
  
  status_display(2);  //Display Information 2 on LCD
  status_display(3);  //Display Information 3 on LCD

  /*  Wait 10 seconds. This gives the GSM module
   *  a period of time to locate and log on to
   *  the cellular network
   */
  delay(10000);
  status_display(4);  //Display Information 4 on LCD
  return;
}


void setup_environment()
{
  //Read in settings from EEPROM or make use of the default settings
  read_settings();

  //GSM shield initialization
  //Start the software Serial to interface the GSM shield (SWSerial) with the Arduino board
  gsm_shield.begin(19200);  //using baud rate of 19200 for the serial TX and RX
  gsm_shield.println(F("AT+CMGF=1"));  // set SMS mode to text
  delay(100);

  // blurt out contents of new SMS upon receipt to the GSM shield's serial out
  gsm_shield.println(F("AT+CNMI=2,2,0,0,0"));
  delay(100);

  //ADC initialization
  analogReference(DEFAULT); //Use the default reference voltage (5V/3.3V)
  
#ifdef DEBUG
  Serial.println(F("Setup Complete"));
  Serial.print(F("AUTO POWER: "));
  Serial.println(gAuto_Power);
  Serial.print(F("AUTO ACK: "));
  Serial.println(gAuto_Ack);
  Serial.print(F("PIN: "));
  Serial.println(gPIN);
  Serial.print(F("TEMP 1: "));
  Serial.println(gCompartment1_Temp_Limit);
  Serial.print(F("TEMP 2: "));
  Serial.println(gCompartment2_Temp_Limit);
  Serial.println('\n');
#endif

  status_display(5);  //Display Information 5 on LCD
  return;
}


