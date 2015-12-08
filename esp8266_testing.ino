#include <ESP8266WiFi.h>
 
const char* ssid     = "yourssid";
const char* password = "yourpassword";

const char* timeHost = "time.nist.gov";
const char* dataHost = "data.sparkfun.com";


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
}

String UtcDateTime()
{
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int timePort = 13;

  if (!client.connect(timeHost, timePort)) {
    return "Connection to NIST failed";
  }  
  
  // Send the request to the server
  client.print("HEAD / HTTP/1.1\r\nAccept: */*\r\nUser-Agent: Mozilla/4.0 (compatible; ESP8266 NodeMCU Lua;)\r\n\r\n");
  delay(100);
  String dateTime = "";

  // Read all the lines of the reply from server
  // expected line is like : Date: Thu, 01 Jan 2015 22:00:14 GMT
  while(client.available()) {
    String line = client.readStringUntil('\r');
    if (line.indexOf("UTC(NIST)") == -1)
    {
      dateTime = "Unexpected response from NIST";
    }
    else
    {
      // date starts at pos 7
      dateTime = line.substring(7, 24) + " UTC";
    }
  }

  return dateTime;
}

void loop() {
  String dateTime = UtcDateTime();
  Serial.println(dateTime);

  delay(60000);
}
