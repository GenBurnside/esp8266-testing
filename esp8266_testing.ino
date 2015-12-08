#include <ESP8266WiFi.h>
 
const char* ssid     = "yourssid";
const char* password = "yourpassword";

const char* timeHost = "time.nist.gov";
const char* dataHost = "data.sparkfun.com";

void setup() {
  Serial.begin(115200);
  delay(100);
 
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}
 
void loop() {
  Serial.print("connecting to ");
  Serial.println(timeHost);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int timePort = 13;
  const int dataPort = 80;

  if (!client.connect(timeHost, timePort)) {
    Serial.println("connection failed");
    return;
  }  
  
  // Send the request to the server
  client.print("HEAD / HTTP/1.1\r\nAccept: */*\r\nUser-Agent: Mozilla/4.0 (compatible; ESP8266 NodeMCU Lua;)\r\n\r\n");
  delay(100);

  // Read all the lines of the reply from server and print them to Serial
  // expected line is like : Date: Thu, 01 Jan 2015 22:00:14 GMT
  while(client.available()){
    String line = client.readStringUntil('\r');
    if (line.indexOf("UTC(NIST)") == -1)
    {
      Serial.println("Unexpected response from NIST");
    }
    else
    {
      // date starts at pos 7
      String dateTime = line.substring(7, 24) + " UTC";
      Serial.println(dateTime);
    }
  }
  
  Serial.println();
  Serial.println("closing connection");

  delay(60000);
}
