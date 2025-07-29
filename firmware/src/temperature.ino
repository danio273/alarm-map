#include "DHT.h"

DHT* dht;

void initTemperatureSensor() {
  Serial.println("Ініціалізація датчика температури...");

  if (temperatureType == 11) dht = new DHT(temperaturePin, DHT11);
  else if (temperatureType == 21) dht = new DHT(temperaturePin, DHT21);
  else if (temperatureType == 22) dht = new DHT(temperaturePin, DHT22);

  dht->begin();
}

// void handleTemperaturesensor() {
//   SinricProTemperaturesensor &mySensor = SinricPro[sinricTemperatureId];
//   mySensor.sendTemperatureEvent(getTemperature(), getHumidity());
// }

float getTemperature() {
  return dht->readTemperature() + temperatureOffset;
}

float getHumidity() {
  return dht->readHumidity();
}

int checkConditions(float temperature, float humidity) {
  if ((temperature < 19 && humidity < 30) ||
      (temperature < 19 && humidity > 70) ||
      (temperature > 27 && humidity < 30) ||
      (temperature > 27 && humidity > 70)) return 0;

  if ((temperature >= 19 && temperature <= 27 && humidity < 30) ||
      (temperature >= 19 && temperature <= 27 && humidity > 70) ||
      (temperature < 19 && humidity >= 30 && humidity <= 70) ||
      (temperature > 27 && humidity >= 30 && humidity <= 70)) return 1;

  if (temperature >= 19 && temperature <= 27 && humidity >= 30 && humidity <= 70)  return 2;
}