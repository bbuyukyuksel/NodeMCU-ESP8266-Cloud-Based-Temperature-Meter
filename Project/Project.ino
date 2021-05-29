#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Parametrik yapılacak.
const char* PostServerAddress = "http://192.168.1.7:5000/login";
//0;Node-01;EloHab24;3v3@Geldi;None;None;None
/*
  //Mode:0
  set:Device Name
  set:SSID
  set:PASSWD
  //Mode:1
  set:Device Name
  set:SSID
  set:PASSWD
  set:STATIC_IP
  set:GATEWAY
  set:DNS
*/

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

uint8_t PinLed_buildin = 2;

#define C_MODE 0
#define C_DEVICE_NAME 1
#define C_SSID 2
#define C_PASS 3
#define C_STATIC_IP 4
#define C_GATEWAY 5
#define C_SUBNET 6

// Request Params
unsigned long POST_RequestLastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long POST_RequestTimerDelay = 5000;

// Device Configs
char DEVICE_CONFIG[128] = {};
String CONF[7];

WiFiServer server(80);

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
      is_set_device_config = true;
    }
  }

  if (!is_set_device_config ) {
    EEPROM.get(0, DEVICE_CONFIG);
    Serial.println(DEVICE_CONFIG);
  }

  Serial.println("Configuration timeout!");

  String D_CONF = String(DEVICE_CONFIG);

  for (uint8_t i = 0; i < 7; ++i) {
    CONF[i] = getValue(D_CONF, ';', i);
    Serial.println(CONF[i]);
  }

  pinMode(PinLed_buildin, OUTPUT);
  digitalWrite(PinLed_buildin, LOW);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true); // don't continue
  }
  //WiFi.hostname(deviceName);
  //WiFi.config(staticIP, gateway, subnet);

  switch (CONF[C_MODE].toInt()) {
    case 0 : Serial.println("Mode:0"); break;
    case 1 : Serial.println("Mode:1"); break;
  }

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(CONF[C_SSID]);

  WiFi.begin(CONF[C_SSID], CONF[C_PASS]);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}


void loop() {

  // Block: Web Serve
  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
    // Wait until the client sends some data
    Serial.println("new client");
    while (!client.available()) {
      delay(1);
    }

    // Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    // Match the request

    int value = LOW;
    Serial.println(request.indexOf("/LED=ON"));
    if (request.indexOf("/LED=ON") != -1)  {
      digitalWrite(PinLed_buildin, LOW);
      value = HIGH;
    }
    if (request.indexOf("/LED=OFF") != -1)  {
      digitalWrite(PinLed_buildin, HIGH);
      value = LOW;
    }


    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");

    client.print("Led pin is now: ");

    if (value == HIGH) {
      client.print("On");
    } else {
      client.print("Off");
    }
    client.println("<br><br>");
    client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
    client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");
    client.println("</html>");

    delay(1);
    Serial.println("Client disonnected");
    Serial.println("");
  }

  // End-Block:Web Serve

  // Block: Send Post Request
  //Send an HTTP POST request every 10 minutes
  if ((millis() - POST_RequestLastTime) > POST_RequestTimerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin(PostServerAddress);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpRequestData = "api=nodewithme001&device_name=" + CONF[C_DEVICE_NAME];
      int httpResponseCode = http.POST(httpRequestData);

      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

      if (httpResponseCode>0) {
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    POST_RequestLastTime = millis();
  }
  // End-Block: Send Post Request
  
  


}
