#include <ESP8266WiFi.h>
 
const char* ssid     = "SSID";
const char* password = "PASSWORD";

const char* dataHost = "data.sparkfun.com";

//https://data.sparkfun.com/streams/PUBLICKEY
const char* dataPubKey  = "PUBLICKEY";
const char* dataPrivKey = "PRIVATEKEY";

void setup() {
  WiFi.begin(ssid, password);
}

void PostToDataStream(String temperature, String humidity, String light)
{
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(dataHost, 80)) {
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

  // Send the request to the server
  client.print(request);
  delay(500);
}

float GetTemperature()
{
  int reading = analogRead(A0);

  // convert reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 3.3;
  voltage /= 1024.0;

  float temperatureC = (voltage - 0.5) * 100;
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;

  temperatureF += 0.5; //for rounding to int
  return (int)temperatureF;
}

String FormatTemperature(int input)
{
  String output = String(input) + "%C2%B0%20F";
  return output;
}

String FormatHumidity(int input)
{
  String output = String(input) + "%25";
  return "-";
}

String FormatLight(int input)
{
  String lightLevels[] = { "low", "medium", "high" };
  return "-";
}

void loop() {
  int tVal = GetTemperature();

  String temperature = FormatTemperature(tVal);
  String humidity = FormatHumidity(0);
  String light = FormatLight(0);

  PostToDataStream(temperature, humidity, light);
  
  delay(60000);
}
