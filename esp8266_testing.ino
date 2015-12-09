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

void PostToDataStream(String temperature, String humidity, String light)
{
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(dataHost, 80)) {
    Serial.println("Connection to SparkFun failed");
    return;
  }

  String url = "/input/" + String(dataPubKey);

  String body = "temperature=" + temperature +
                "&humidity=" + humidity +
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

String FormatTemperature(int input)
{
  String output = String(input) + "%C2%B0%20F";
  return output;
}

String FormatHumidity(int input)
{
  String output = String(input) + "%25";
  return output;
}

String FormatLight(int input)
{
  String lightLevels[] = { "low", "medium", "high" };
  return lightLevels[input];
}

void loop() {
  int tVal = random(30, 101);
  int hVal = random(0, 101);
  int lVal = random(3);

  String temperature = FormatTemperature(tVal);
  String humidity = FormatHumidity(hVal);
  String light = FormatLight(lVal);

  PostToDataStream(temperature, humidity, light);
  
  delay(60000);
}
