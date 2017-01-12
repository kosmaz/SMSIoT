#include "Definitions.hpp"


void acknowledge_command(String command)
{
  String msg_part1 = F("Command ");
  String msg_part2 = F( " Acknowledged");
  send_sms(msg_part1 + command + msg_part2);
  return;
}


void acknowledge_same_setting(String setting)
{
  String msg_part1 = F("The value given for ");
  String msg_part2 = F(" is the present value being used");
  send_sms(msg_part1 + setting + msg_part2);
  return;
}


void acknowledge_same_state(String state)
{
  String msg_part = F("Refrigerator is already powered ");
  send_sms(msg_part + state);
  return;
}


void unacknowledge_command(String command)
{
  String msg_part1 = F("Command "); 
  String msg_part2 = F(" not Acknowledged");
  send_sms(msg_part1 + command + msg_part2);
  return; 
}


