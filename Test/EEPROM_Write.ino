// Writer
#include <EEPROM.h>
 
int addr = 0;
char DEVICE_CONFIG[128] = "Hello World!";

void setup()
{
  EEPROM.begin(512);  //Initialize EEPROM
 
  // write to EEPROM.
  EEPROM.put(addr, DEVICE_CONFIG);    
  EEPROM.commit();    //Store data to EEPROM
}
 
void loop()
{   
}
