#include <EEPROM.h>
//Node-01;EloHab24;3v3@Geldi;None;None;None
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

char DEVICE_CONFIG[128] = {};
String CONF[6];

#define C_DEVICE_NAME 0
#define C_SSID 1
#define C_PASS 2
#define C_STATIC_IP 3
#define C_GATEWAY 4
#define C_SUBNET 5

/*
  device_name;ssid;pass;static_ip;gateway;subnet;
*/

void setup() {
  bool is_set_device_config = false;
  
  EEPROM.begin(512);  //Initialize EEPROM
  Serial.begin(115200);
  while (!Serial);
  Serial.flush();
  Serial.setTimeout(100);  // bekleme zamanını 100 ms olarak ayarla.

  // Set Configuration and Write to EEPROM
  while (millis() < 10000) {
    if (Serial.available()) { // Serial Porta girdi değerinin olup olmadığını kontrol et
      // eğer girdi varsa oku ve yazdır.

      String tempDeviceConfig = Serial.readStringUntil('\r\n'); // Serial Porttan değer okuma
      const char *str = tempDeviceConfig.c_str();

      strcpy_P(DEVICE_CONFIG, str);
      
      Serial.println("EEPROM Hafıza: " + String(EEPROM.length()));
      // write to EEPROM.
      EEPROM.put(0, DEVICE_CONFIG);    
      EEPROM.commit();    //Store data to EEPROM
      bool is_set_device_config = true;
    }
  }

  if(!is_set_device_config ){
    EEPROM.get(0, DEVICE_CONFIG);
    Serial.println(DEVICE_CONFIG);
  }
  
  Serial.println("Configuration timeout!");

  String D_CONF = String(DEVICE_CONFIG);

  for(uint8_t i=0; i<6; ++i){
    CONF[i] = getValue(D_CONF, ';', i);
    Serial.println(CONF[i]);
  }

  
}


void loop() {

}
