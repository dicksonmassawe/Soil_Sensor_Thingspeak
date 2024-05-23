#include <WiFi.h>
#include <HTTPClient.h>
#include "Credentials.h"

#ifndef sensor
#define sensor Serial1
#endif

// Declare global variables to hold sensor data
unsigned int soilHumidity = 0;
unsigned int soilTemperature = 0;
float soilConductivity = 0;
unsigned int soilPH = 0;
float nitrogen = 0;
unsigned int phosphorus = 0;
float potassium = 0;

// LED and  Buzzer
#define white1 34
#define white2 35
#define red 25
#define blue 13
#define green 2
#define buzzer 14

void setup() {
  Serial.begin(115200);
  sensor.begin(4800, SERIAL_8N1, 33, 32);  // Start hardware serial at 4800 bps

  // LED and Buzzer
  pinMode(white1, OUTPUT);
  pinMode(white2, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);

  digitalWrite(white1, 0);
  digitalWrite(white2, 0);
  digitalWrite(red, 0);
  digitalWrite(blue, 0);
  digitalWrite(green, 0);
  digitalWrite(buzzer, 0);

  // Connecting to WiFi
  connectToWiFi();

  Serial.println("Setup complete. Waiting for sensor data...");
}



void loop() {

  // Readind sensor Data
  readSensorData();

  // Send data to Cloud
  unsigned int humd = soilHumidity / 10;
  unsigned int temp = soilTemperature / 10;
  float cond = soilConductivity / 1000;
  unsigned int ph = soilPH / 10;
  float nitro = nitrogen / 10000;
  unsigned int phos = phosphorus;
  float pota = potassium / 390;
  sendToThingSpeak(humd, temp, cond, ph, nitro, phos, pota);

  // Data Delay
  delay(16000);
}


void readSensorData() {
  byte queryData[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08 };
  byte receivedData[19];

  sensor.write(queryData, sizeof(queryData));  // Send query data to NPK
  Serial.println("Query sent to sensor.");

  if (sensor.available() >= sizeof(receivedData)) {        // Check if there are enough bytes available to read
    sensor.readBytes(receivedData, sizeof(receivedData));  // Read the received data into the receivedData array

    // Parsing and printing the received data in decimal format
    soilHumidity = (receivedData[3] << 8) | receivedData[4];
    soilTemperature = (receivedData[5] << 8) | receivedData[6];
    soilConductivity = (receivedData[7] << 8) | receivedData[8];
    soilPH = (receivedData[9] << 8) | receivedData[10];
    nitrogen = (receivedData[11] << 8) | receivedData[12];
    phosphorus = (receivedData[13] << 8) | receivedData[14];
    potassium = (receivedData[15] << 8) | receivedData[16];

    Serial.print("Soil Humidity: ");
    Serial.print((float)soilHumidity / 10.0);
    Serial.print(" %    ");
    Serial.print("Soil Temperature: ");
    Serial.print((float)soilTemperature / 10.0);
    Serial.print(" °C    ");
    Serial.print("Soil Conductivity: ");
    Serial.print((float)soilConductivity / 1000);
    Serial.print(" dS/m    ");  // microsiemens per centimeter (µS/cm)
    Serial.print("Soil pH: ");
    Serial.print((float)soilPH / 10.0);
    Serial.print("   ");
    Serial.print("Nitrogen: ");
    Serial.print(nitrogen / 10000);
    Serial.print(" %    ");
    Serial.print("Phosphorus: ");
    Serial.print(phosphorus);
    Serial.print(" mg/kg    ");
    Serial.print("Potassium: ");
    Serial.print(potassium / 390);
    Serial.print(" cmol(+)/kg \n");
  } else {
    Serial.println("No data available from sensor.");
  }
}

void sendToThingSpeak(unsigned int humidity, unsigned int temperature, unsigned int conductivity, unsigned int pH, float nitrogen, unsigned int phosphorus, float potassium) {
  String url = "https://api.thingspeak.com/update?api_key=";
  url += thingspeak_write_api_key;

  url += "&field1";
  url += "=";
  url += String(humidity);

  url += "&field2";
  url += "=";
  url += String(temperature);

  url += "&field3";
  url += "=";
  url += String(conductivity);

  url += "&field4";
  url += "=";
  url += String(pH);

  url += "&field5";
  url += "=";
  url += String(nitrogen);

  url += "&field6";
  url += "=";
  url += String(phosphorus);

  url += "&field7";
  url += "=";
  url += String(potassium);

  Serial.println(url);

  HTTPClient http;
  http.begin(url);

  int httpCode = http.GET();
  String payload = http.getString();

  Serial.print("httpCode: ");
  Serial.println(httpCode);
  Serial.print("payload: ");
  Serial.println(payload);
  Serial.println("");

  if (httpCode == 200) buzzerSuccess(green);
  else buzzerSuccess(red);

  http.end();
}

void connectToWiFi() {
  WiFi.begin(mySSID, myPASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    digitalWrite(blue, HIGH);
    delay(1000);
    digitalWrite(blue, LOW);
  }

  Serial.println("Connected to WiFi");
  buzzerSuccess(blue);
}

void buzzerError(int led) {
  for (int i = 0; i < 10; ++i) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(buzzer, LOW);
    delay(50);
  }
  digitalWrite(led, LOW);
}

void buzzerSuccess(int led) {
  for (int i = 0; i < 3; ++i) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(500);
  }
  digitalWrite(led, LOW);
}