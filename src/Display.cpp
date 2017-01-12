#include "Definitions.hpp"

LiquidCrystal lcd_display(2, 3, 10, 11, 12, 13);

void status_display(uint8_t info)
{
  /*  Display information based on info number
   *  Info 0: Initial display
   *  Info 1: Turning on GSM/GPRS module
   *  Info 2: GSM/GPRS module turned on
   *  Info 3: Registering SIM card to network
   *  Info 4: SIM card network registration complete
   *  Info 5: Display the skeletal strings for 
   *          temperature 1 & 2 as well as power status
   *  Info 6: Display the temperature readings for 
   *          compartment 1 & 2 as well as a YES/NO
   *          word to the power status.
   */
  switch(info)
  {
    case 0:
    {
      lcd_display.setCursor(0, 0);
      lcd_display.print(F("Starting Module>"));
      lcd_display.setCursor(0, 1);
      lcd_display.print(F("Designed by:    "));
      lcd_display.setCursor(0, 2);
      lcd_display.print(F("EZENWANNE I. C. "));
      lcd_display.setCursor(0, 3);
      lcd_display.print(F("   2011364065   "));
      delay(1500);
      lcd_display.setCursor(0, 2);
      lcd_display.print(F("NRIAGU JOSEMARIA"));
      lcd_display.setCursor(0, 3);
      lcd_display.print(F("   2011364079   "));
      delay(1500);
      lcd_display.setCursor(0, 2);
      lcd_display.print(F(" EKWUNIFE S. N. "));
      lcd_display.setCursor(0, 3);
      lcd_display.print(F("   2010364117   "));
      delay(1500);
      break;
    }
    
    case 1:
    {
      lcd_display.clear();
      lcd_display.setCursor(0, 0);
      lcd_display.print(F("   Turning on   "));
      lcd_display.setCursor(0, 1);
      lcd_display.print(F("GSM/GPRS module "));
      lcd_display.setCursor(0, 2);
      lcd_display.print(F("..>"));
      delay(500);
      lcd_display.print(F(".....>"));
      delay(500);
      lcd_display.print(F("......>"));
      break;
    }

    case 2:
    {
      lcd_display.clear();
      lcd_display.print(F("GSM/GPRS module "));
      lcd_display.setCursor(0, 1);
      lcd_display.print(F("    Turned on   "));
      lcd_display.setCursor(0, 2);
      lcd_display.print(F("successfully !!!"));
      delay(2000);
      break;    
    }

    case 3:
    {
      lcd_display.clear();
      lcd_display.print(F("   Registering  "));
      lcd_display.setCursor(0, 1);
      lcd_display.print(F("   SIM card to  "));
      lcd_display.setCursor(0, 2);
      lcd_display.print(F("   network...   "));
      break;
    }

    case 4:
    {
      lcd_display.clear();
      lcd_display.print(F("    SIM card    "));
      lcd_display.setCursor(0, 1);
      lcd_display.print(F("  successfully  "));
      lcd_display.setCursor(0, 2);
      lcd_display.print(F("  registered to "));
      lcd_display.setCursor(0, 3);
      lcd_display.print(F("   network !!!  "));
      delay(2000);
      break;
    }

    case 5:
    { 
      lcd_display.clear();
      lcd_display.print(F(" Module Running "));
      lcd_display.setCursor(0, 1);
      lcd_display.print(F("TEMP 1:         "));
      lcd_display.setCursor(0, 2);
      lcd_display.print(F("TEMP 2:         "));
      lcd_display.setCursor(0, 3);
      lcd_display.print(F("POWERED ON:     "));
      break;
    }

    case 6:
    {
      lcd_display.setCursor(14, 1);
      lcd_display.print("  ");
      lcd_display.setCursor(9, 1);
      lcd_display.print(temperature_read(1));
      lcd_display.setCursor(14, 2);
      lcd_display.print("  ");
      lcd_display.setCursor(9, 2);
      lcd_display.print(temperature_read(2));
      lcd_display.setCursor(13, 3);
      if(gPower_On)
        lcd_display.print(F("YES"));
      else
        lcd_display.print(F("NO "));
       delay(500);
      break;
    }

    default: break;
  }
  return;
}


float _temperature_read(uint8_t compartment)
{
  float temp = 0.0;
  switch(compartment)
  {
    case 1:
    {
      //read the temperature from the sensor 10 times to improve accuracy
      for(int i = 0; i < 10; ++i)
      {
        /*  In other to be able to read negative temperature values from
         *  the LM35 sensor we need to figure follow the digram in the
         *  LM35 datasheet in other to have to seperate inputs to the
         *  microcontroller. One for Vout and the other for Vref seen
         *  by the LM35 sensor. We feed this two values from the sensor 
         *  into two seperate ADC channels on the microcontroller and then
         *  take our readings from both channels. In other to be able to
         *  get a negative reading when available from the sensor especially
         *  in our area of application (Refrigerator) where temperatures
         *  can go as low as less that 0 degree celcius, we subtract the 
         *  ADC reading of the Vref output from the ADC reading of the Vout
         *  output.
         */
        temp += (analogRead(COMPARTMENT1) / 2.0) - (analogRead(COMPARTMENT1_REF) / 2.0);
        delay(10);
      }

      temp /= 10.0;
      break;
    }

    case 2:
    {
      //read the temperature from the sensor 10 times to improve accuracy
      for(int i = 0; i < 10; ++i)
      {
        /*  In other to be able to read negative temperature values from
         *  the LM35 sensor we need to figure follow the digram in the
         *  LM35 datasheet in other to have to seperate inputs to the
         *  microcontroller. One for Vout and the other for Vref seen
         *  by the LM35 sensor. We feed this two values from the sensor 
         *  into two seperate ADC channels on the microcontroller and then
         *  take our readings from both channels. In other to be able to
         *  get a negative reading when available from the sensor especially
         *  in our area of application (Refrigerator) where temperatures
         *  can go as low as less that 0 degree celcius, we subtract the 
         *  ADC reading of the Vref output from the ADC reading of the Vout
         *  output.
         */
        temp += (analogRead(COMPARTMENT2) / 2.0) - (analogRead(COMPARTMENT2_REF) / 2.0);
        delay(10);
      }

      temp /= 10.0;
      break;
    }

    default: break;
  }
  return temp;
}


