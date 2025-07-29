// bool onPowerState(const String &deviceId, bool &state) {
//   if (state) turnOnStrip();
//   else turnOffStrip();
//   #ifdef DEBUG
//     Serial.printf("Пристрій %s %s \r\n", deviceId.c_str(), state?"ввімкнено":"вимкнено");
//   #endif
//   return true; // request handled properly
// }

// bool onBrightness(const String &deviceId, int &brightness) {
//   changeBrightness(brightness, false);
//   turnOnStrip();
//   #ifdef DEBUG
//     Serial.printf("Пристрою %s встановлено яскравість %d\r\n", deviceId.c_str(), brightness);
//   #endif
//   return true;
// }

// bool onAdjustBrightness(const String &deviceId, int brightnessDelta) {
//   changeBrightness(brightness + brightnessDelta, false);
//   turnOnStrip();
//   #ifdef DEBUG
//     Serial.printf("Пристрою %s збільшено яскравість на %i (нове занчення %d)\r\n", deviceId.c_str(), brightnessDelta, brightness);
//   #endif
//   return true;
// }

// void initSinricPro() {
//   Serial.println("Ініціалізація Sinric Pro...");

//   SinricPro.onConnected([](){Serial.println("Підключено до SinricPro.");}); 
//   SinricPro.onDisconnected([](){Serial.println("Відключено від SinricPro.");});
//   SinricPro.begin(sinricKey, sinricSecret);

//   if (!sinricStripId.isEmpty()) {
//     SinricProDimSwitch &sinricStrip = SinricPro[sinricStripId];

//     sinricStrip.onPowerState(onPowerState);
//     sinricStrip.onPowerLevel(onBrightness);
//     sinricStrip.onAdjustPowerLevel(onAdjustBrightness);

//     sinricStrip.sendPowerStateEvent(enabledStrip);
//     sinricStrip.sendPowerLevelEvent(brightness);
//   }

//   if (!sinricTemperatureId.isEmpty()) {
//     SinricProTemperaturesensor &sinricTemperature = SinricPro[sinricTemperatureId];
//     sinricTemperature.onPowerState(onPowerState);
//   }
// }