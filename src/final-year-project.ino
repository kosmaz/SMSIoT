#include "Definitions.hpp"


uint8_t gPower_On = FALSE;
uint8_t gIs_Value_Changed = FALSE;
int8_t gValue_Changed = -1;
String gNumber = "";
uint8_t gAuto_Power = DEFAULT_AUTO_POWER;
uint8_t gAuto_Ack = DEFAULT_AUTO_ACK;
String gPIN = DEFAULT_PIN;
float gCompartment1_Temp_Limit = NO_TEMP_LIMIT;
float gCompartment2_Temp_Limit = NO_TEMP_LIMIT;


/*************CODE EXECUTION STARTS HERE*******************/

void setup()
{
  pinMode(LED_1, OUTPUT); //LED 1 OUTPUT PIN  
  //send a HIGH to turn ON led which indicates POWER IN from mains
  digitalWrite(LED_1, HIGH);  
  
  pinMode(LED_2, OUTPUT); //LED 2 OUTPUT PIN
  //send LOW to turn OFF led which indicates no POWER OUT to refrigerator
  digitalWrite(LED_2, LOW); 
  
  pinMode(LED_3, OUTPUT); //LED 3 OUTPUT PIN
  //send LOW to turn OFF led which indicates no SMS recieved by the GSM shield
  digitalWrite(LED_3, LOW); 
  
  pinMode(RELAY_CONTROL, OUTPUT); //RELAY OUTPUT CONTROL PIN
  //send LOW to OPEN the relay circuit switch
  digitalWrite(RELAY_CONTROL, LOW);
  
  pinMode(PWRKEY, OUTPUT);  //GSM SHIELD POWER CONTROL PIN

  //lcd initialization
  lcd_display.begin(16, 4);  //initialize LCD with 16 columns and 4 rows
  lcd_display.noBlink();  //disable cursor blinking
  lcd_display.noCursor(); //disable cursor
  status_display(0);  //Display Information 0 on LCD
  
  toggle_shield_power();
  setup_environment();
    
  return;
}


void loop()
{
  master_controller();
  return;
}

/******************CODE EXECUTION ENDS HERE************************/


