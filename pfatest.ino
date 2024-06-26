#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define DHTTYPE DHT11
const char* ssid = "Bn";
const char* password = "11111111";
const char* serverName = "http://192.168.43.14:8000/api/post";
#define dht_dpin D1
DHT dht(dht_dpin, DHTTYPE);

void setup() {
  dht.begin();
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Formater la température avec deux chiffres après la virgule
  String temperatureStr = String(temperature, 2);

  Serial.print("Current humidity = ");
  Serial.print(humidity);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(temperatureStr); 
  Serial.println("C  ");

  // Envoi des valeurs au serveur Web
  WiFiClient client;
  HTTPClient http;
  DynamicJsonDocument jsonDoc(200);
  jsonDoc["temp"] = temperatureStr;
  jsonDoc["hum"] = humidity;
  String jsonStr;
  serializeJson(jsonDoc, jsonStr);

  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/json");
  Serial.println(jsonStr);
  int httpResponseCode = http.POST(jsonStr);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();

  // Attente de 1 secondes avant de lire les valeurs suivantes
  delay(1000);
}
