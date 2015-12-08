#include <ESP8266WiFi.h>
 
const char* ssid     = "SSID";
const char* password = "PASSWORD";

const char* dataHost = "data.sparkfun.com";

//https://data.sparkfun.com/streams/PUBLICKEY
const char* dataPubKey  = "PUBLICKEY";
const char* dataPrivKey = "PRIVATEKEY";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
}

void PostToDataStream(int temperature, int humidity, String light)
{
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(dataHost, 80)) {
    Serial.println("Connection to SparkFun failed");
    return;
  }

  String url = "/input/" + String(dataPubKey);

  String body = "temperature=" + String(temperature) + "%C2%B0%20F" +
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
  int temperature = random(30, 101);
  
  int humidity = random(0, 101);

  String lightLevels[] = { "low", "medium", "high" };
  int index = random(3);
  String light = lightLevels[index];

  PostToDataStream(temperature, humidity, light);
  
  delay(60000);
}
