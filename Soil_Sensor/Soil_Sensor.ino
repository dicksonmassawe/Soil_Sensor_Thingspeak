#ifndef sensor
#define sensor Serial1
#endif

void setup() {
  Serial.begin(115200);
  sensor.begin(4800, SERIAL_8N1, 33, 32); // Start hardware serial at 4800 bps
  Serial.println("Setup complete. Waiting for sensor data...");
}

void loop() {
  byte queryData[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08 };
  byte receivedData[19];

  sensor.write(queryData, sizeof(queryData)); // Send query data to NPK
  Serial.println("Query sent to sensor.");
  delay(1000);

  if (sensor.available() >= sizeof(receivedData)) { // Check if there are enough bytes available to read
    sensor.readBytes(receivedData, sizeof(receivedData)); // Read the received data into the receivedData array

    // Parsing and printing the received data in decimal format
    unsigned int soilHumidity = (receivedData[3] << 8) | receivedData[4];
    unsigned int soilTemperature = (receivedData[5] << 8) | receivedData[6];
    unsigned int soilConductivity = (receivedData[7] << 8) | receivedData[8];
    unsigned int soilPH = (receivedData[9] << 8) | receivedData[10];
    float nitrogen = (receivedData[11] << 8) | receivedData[12];
    unsigned int phosphorus = (receivedData[13] << 8) | receivedData[14];
    float potassium = (receivedData[15] << 8) | receivedData[16];

    Serial.print("Soil Humidity: ");
    Serial.print((float)soilHumidity / 10.0);
    Serial.print(" %    ");
    Serial.print("Soil Temperature: ");
    Serial.print((float)soilTemperature / 10.0);
    Serial.print(" °C    ");
    Serial.print("Soil Conductivity: ");
    Serial.print((float)soilConductivity);
    Serial.print(" µS/cm    ");  // microsiemens per centimeter (µS/cm)
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
    Serial.println(" cmol(+)/kg \n");
  } else {
    Serial.println("No data available from sensor.");
  }


}
