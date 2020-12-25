//Reader
#include <EEPROM.h>
 
int addr = 0;
char DEVICE_CONFIG[128] = {};
void setup()
{
  EEPROM.begin(512);  //Initialize EEPROM
  Serial.begin(9600); 
  Serial.println("");
  EEPROM.get(0, DEVICE_CONFIG);
  Serial.print(char(EEPROM.read(addr)));
  
  Serial.print(DEVICE_CONFIG);  //Print the text
}
 
void loop()
{ 
   Serial.println(DEVICE_CONFIG);  //Print the text
}
