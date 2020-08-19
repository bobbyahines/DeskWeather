// import the Arduino Json and DHT libraries.
#include <ArduinoJson.h>
#include "DHT.h"

// Instaniate the DHT library, and point to Digital Pin 2, and the sensor model (11 vs 22)
DHT dht(2, DHT11);
// Assign the UV sensor to Analog Pin 0
int UVSensorPin = A0;

void setup() {
  // Open up the serial port at 9600 baud
  Serial.begin(9600);
  // Begin streaming via the dht lib
  dht.begin();
  //
  pinMode(UVSensorPin, INPUT);
}

void loop() {
  // Capture the UV sensor stream
  int uvLevel = averageAnalogRead(UVSensorPin); 
  // math the uv level to get the output voltage in mW/cm^2
  float uvOutputVoltage = 5.0 * uvLevel/1024;
  // math the output voltage to convert to intensity
  float uvIntensity = mapfloat(uvOutputVoltage, 0.99, 2.9, 0.0, 15.0);
  // capture the soil moisture stream
  const char* soilMoisture = readSoilMoisture();
  // use th dht library to decipher the stream from the pin
  float fahrenheit = dht.readTemperature(true);
  float celsius = dht.readTemperature();
  float humidity = dht.readHumidity();

  // print the results as a json object
  Serial.print("[{");

  Serial.print("\"soilMoisture\": \"");
  Serial.print(soilMoisture);
  Serial.print("\"");
  
  Serial.print(", ");
  
  Serial.print("\"fahrenheit\": ");
  Serial.print(dht.readTemperature(true));
  
  Serial.print(", ");

  Serial.print("\"celsius\": ");
  Serial.print(dht.readTemperature());
  
  Serial.print(", ");
  
  Serial.print("\"humidity\": ");
  Serial.print(dht.readHumidity());
  
  Serial.print(", ");
  
  Serial.print("\"uvIntensity\": ");
  Serial.print(uvIntensity);
  
  Serial.print(", ");
  
  Serial.print("\"outputVoltage\": ");
  Serial.print(uvOutputVoltage);
  
  Serial.print("}]");
  
  Serial.println();

  // Sleep 5 seconds
  delay(5000);
}

const char* readSoilMoisture() {
  
  float sm = analogRead(A1);
  
  if (sm > 0 && sm <= 300) {
    return "Dry";
  } else if (sm > 300 && sm <= 700) {
    return "OK!";
  } else if (sm > 700) {
    return "Wet";
  } else {
    return "Err";
  }  
}

//Takes an average of readings on a given pin
//Returns the average
int averageAnalogRead(int pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0;
  
  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);

}

//The Arduino Map function but for floats
//From: http://forum.arduino.cc/index.php?topic=3922.0
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
