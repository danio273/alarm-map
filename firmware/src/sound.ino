#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini speaker;

void initBuzzer() {
  Serial.println("Ініціалізація бузера...");
  pinMode(buzzerPin, OUTPUT); 
}

void initSpeaker() {
  Serial.println("Ініціалізація динаміка...");

  Serial2.begin(9600);

  speaker.begin(Serial2);

  speaker.volume(speakerVolume * 0.3);
  
  speaker.outputDevice(DFPLAYER_DEVICE_SD);
}

void buzzerAlarmSound(bool homeCityEnable) {
  #ifdef DEBUG
    Serial.println("Програвання тривоги/відбою на бузері...");
  #endif

  while (true) {
    if (homeCityEnable) {
      tone(buzzerPin, 1000); 
      delay(1000); 
      noTone(buzzerPin); 
      delay(500);
    } else {
      tone(buzzerPin, 1000); 
      delay(200); 
      noTone(buzzerPin); 
      delay(500);
    }
    if (millis() - startTime >= buzzerInterval) break;
  }
}

void speakerAlarmSound(bool homeCityEnable) {
  #ifdef DEBUG
    Serial.println("Програвання тривоги/відбою на динаміку...");
  #endif

  if (homeCityEnable) speaker.play(1);
  else speaker.play(2);
}