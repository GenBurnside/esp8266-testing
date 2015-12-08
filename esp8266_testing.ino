#include <ESP8266WiFi.h>
 
const char* ssid     = "SSID";
const char* password = "PASSWORD";

const char* timeHost = "time.nist.gov";
const char* dataHost = "data.sparkfun.com";

//https://data.sparkfun.com/streams/PUBLICKEY
const char* dataPubKey  = "PUBLICKEY";
const char* dataPrivKey = "PRIVATEKEY";

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

void PostToDataStream(String dateTime, int temperature, int humidity, String light)
{
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(dataHost, 80)) {
    Serial.println("Connection to SparkFun failed");
    return;
  }

  String url = "/input/" + String(dataPubKey);

  String body = "datetime=" + dateTime +
                "&temperature=" + String(temperature) + "%C2%B0%20F" +
                "&humidity=" + String(humidity) + "%25" + 
                "&light=" + light;  
  
  String request  = "POST " + url + " HTTP/1.1\r\n" +
                    "Host: " + dataHost + "\r\n" + 
                    "User-Agent: Mozilla/4.0 (compatible; ESP8266 NodeMCU Lua;)\r\n" +
                    "Content-Type: application/x-www-form-urlencoded\r\n"
                    "Content-Length: " + String(body.length()) + "\r\n" + 
                    "Phant-Private-Key: " + dataPrivKey +
                    "\r\n\r\n" + body;
  
  Serial.println(request);
  Serial.println("");
  
  // Send the request to the server
  client.print(request);

  delay(500);
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}

void loop() {
  String dateTime = UtcDateTime();
  dateTime.replace(" ", "%20");
  Serial.println(dateTime);

  int temperature = random(30, 101);
  int humidity = random(0, 101);

  String lightLevels[] = { "low", "medium", "high" };
  int index = random(3);
  String light = lightLevels[index];

  PostToDataStream(dateTime, temperature, humidity, light);
  
  delay(10000);
}
