#include "Definitions.hpp"

SoftwareSerial gsm_shield(7, 8);


static inline void delete_all_sms()
{
  gsm_shield.println(F("AT+CMGD=1,4")); // delete all SMS
  return;
}


uint8_t incoming_sms()
{
  return gsm_shield.available() > 0;
}


String read_sms()
{
  String sms_buffer = "";
  
  while(gsm_shield.available() > 0)
  {
    char c = gsm_shield.read();
    sms_buffer += c;
    delay(10);
  }

#ifdef DEBUG
  Serial.println("BODY:" + sms_buffer);
#endif

  /*  Since we are asking the GSM shield to blurt out
   *  the contents of new SMS on receipt to the serial's
   *  out using the AT command AT+CNMI=2,2,0,0,0 we 
   *  check the received output from the GSM shield for
   *  the string '+CMT' which indicates a new SMS and
   *  along with it are the SMS  sender's number, time 
   *  and date of SMS receipt and the body of the SMS
   */
  if(sms_buffer.indexOf(F("+CMT")) >= 0)
  {
    /*  Split the sms_buffer string into the sender's number
     *  and the body of the sms 
     */
    gNumber = "";
    uint8_t yes = TRUE;
    for(int i = sms_buffer.indexOf(F("+CMT")) + 5; yes; ++i)
    {
      /*  Copy out the number from the sms_buffer
       *  string
       */
      if(sms_buffer[i] == '+' && gNumber.length() == 0)
      {
        int j = i;
        while(sms_buffer[j] != '\"')
        {
          gNumber += sms_buffer[j];
          ++j;
        }
      }
  
      /*  Get rid of all strings that come before
       *  CR & LF in sms_buffer string. This is to 
       *  retain only the body of the sms in the 
       *  sms_buffer string
       */
  
      if(sms_buffer[i] == '\r' && sms_buffer[i + 1] == '\n')
      {
        /*  Ensure that we are not trying to access an area
         *  ouside of the sms string in memory to avoid the
         *  program crashing unexpectedly.
         */
        if((i != (int)sms_buffer.length() - 1) || ((i + 1) != (int)sms_buffer.length() - 1))
          sms_buffer = sms_buffer.substring(i + 2);
        sms_buffer.toLowerCase();
        yes = FALSE;
      }
      
    }
        
    /*  To avoid parsing sms strings from advertising
     *  numbers which are noted to be highly distracting
     *  and annoying due to the frequency at which they
     *  are received i.e
     *  33034, 53003, 4500, MTN, ETISALAT etc
     *  These numbers have one thing in common which is
     *  that there numbers are not up to 11 characters.
     *  With this feature we can identify such sms from such
     *  numbers and avoid parsing them for commands.
     *  The number under consideration is in an international
     *  format and only valid for Nigerian network operators
     *  +234(0)GSM NUMBER.
     *  This implies that the length of the number string
     *  should be 14 characters
     */
    if(gNumber.length() < 14)
    {
      delete_all_sms();
      return "";
    }
    
  
    /*  We need to get rid of any unnecessary space or 
     *  carriage return (CR) or line feed (LF) in other
     *  to make the sms string easier to parse for the 
     *  individual commands contained in the sms body
     */
    for(unsigned i = 0; i < sms_buffer.length(); ++i)
      if(sms_buffer[i] == ' ' || sms_buffer[i] == '\r' || sms_buffer[i] == '\n')
      {
        sms_buffer.remove(i, 1);
        --i;
      }
    
#ifdef DEBUG
      Serial.println("NUMBER: " + gNumber);
      Serial.println("SMS: " + sms_buffer);
      Serial.println(sms_buffer.length());
#endif


    /*  We need to ensure that the body of the sms received 
     *  from a valid number actually contains at least one
     *  or more commands ending with the character ';' before
     *  returning it as a valid sms to the master_controller
     *  routine
     */
    uint8_t present = FALSE; 
    if(sms_buffer.length() >= 5)
      for(unsigned i = 0; i < sms_buffer.length(); ++ i)
        if(sms_buffer[i] == ';')
        {
          present = true;
          break;
        }
    if(!present)
    {
      delete_all_sms();
      return "";  
    }
      
    delete_all_sms();
    return sms_buffer;
  }
  else
    /*  We need to get rid of status reports from the GSM
     *  shield since we have no use for them in our operations.
     *  Things like AT command acknowlegdement, erros, extra
     *  details to reports etc.
     *  
     *  Things to get rid of include:
     *  OK
     *  NO CARRIER
     *  AT+CMGF=1
     *  AT+CNMI=2,2,0,0,0  
     *  RING e.t.c
     */
     return "";
}


void send_sms(String& msg)
{
  gsm_shield.println("AT+CMGS=\"" + gNumber + "\""); // recipient's mobile number, in international format
  delay(100);
  gsm_shield.println(msg);  // message to send
  delay(100);
  gsm_shield.println((char)26); // End AT command with a ^Z, ASCII code 26
  delay(100);
  gsm_shield.println();
  delay(5000);  //Wait for 5 seconds for the sms to be sent by the GSM shield
  return;
}


