#define EMPTY ""

#include "variables.h"

int networksCount;

#include <ArduinoJson.h> 
String states[] = {"Закарпатська область", "Івано-Франківська область", "Тернопільська область", "Львівська область", "Волинська область", "Рівненська область", "Житомирська область", "Київська область", "Чернігівська область", "Сумська область", "Харківська область", "Луганська область", "Донецька область", "Запорізька область", "Херсонська область", "АР Крим", "Одеська область", "Миколаївська область", "Дніпропетровська область", "Полтавська область", "Черкаська область", "Кіровоградська область", "Вінницька область", "Хмельницька область", "Чернівецька область", "м. Київ"};
#define STATES_COUNT 26
int statesCount;
bool alarmFirstUpdate = true;

#include <FastBot.h>
FastBot bot;

#include <GyverNTP.h>
GyverNTP ntp;

// #include "SinricPro.h"
// #include "SinricProDimSwitch.h"
// #include "SinricProTemperatureSensor.h"

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(0, 255, NEO_GRB + NEO_KHZ800);

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "icons.h"
Adafruit_SSD1306 display;
int disy;
#define TIME_MODE 0
#define DATE_MODE 1
#define CONDITIONS_MODE 2
#define WEATHER_MODE 3
#define ALARM_INFO_MODE 4
#define WAR_MODE 5
#define CURRENCY_MODE 6

#include <GyverOS.h>
GyverOS<6> OS;

int mode; 
String color; 

int displayMode = -1;
int modesCount = 0;

//Змінні для тривалості і стану тривоги
bool enabledStates[STATES_COUNT];
unsigned long startTime, duration;
bool homeCityEnable, lastEnable;

void setup() {
  Serial.begin(115200); 
  Serial.print("\n\n\n\n"); 

  initEEPROM();

  initStrip(); 

  if (displayPresent) initDisplay();

  if (autoBrightnessSource == 1) {
    Serial.println("Ініціалізація фоторезистора...");
    pinMode(photoresistorPin, INPUT);
  }

  initWiFi();

  if (displayPresent && enabledDisplay) {
    display.clearDisplay();
    display.drawBitmap(0, 0 + disy, symbol, 32, 32, 1);
    display.setTextSize(1);
    displayCenter("SDL Alarm Map", 4, 100, 32);
    displayCenter(utf8cyr("Версія " + version), 16, 100, 32);
    display.display();
  }

  validKey = checkKey(accesKey, checkKeyMessage);
  if (validKey) Serial.println("Пристрій доступний! " + checkKeyMessage);
  else Serial.println("Пристрій заблоковано! " + checkKeyMessage + " За необхідності зверніться в службу підтримки @alarm_map_support (Telegram).");

  if (validKey) {
    if (soundSource == 1) initBuzzer();
    else if (soundSource == 2) initSpeaker();

    if (integrationPresent) {
      Serial.println("Ініціалізація інтеграції додатквого модулю...");
      pinMode(integrationPin, OUTPUT); 
    }

    if (temperaturePresent) initTemperatureSensor();

    initTime();

    // if (!sinricKey.isEmpty()) initSinricPro();

    initOS();
  } else {
    if (displayPresent) {
      display.clearDisplay();
      display.drawBitmap(0, 0 + disy, settings, 32, 32, 1);
      display.setTextSize(1);
      displayCenter("alarm-map.local", 8, 100, 32);
      displayCenter(WiFi.localIP().toString(), 16, 100, 32);
    }
  }

  if (!botToken.isEmpty()) {
    initBot();

    checkUpdates(updateInfo);
    if (updateInfo[0].toInt() > versionNumber) {
      String message = "<b>🆕 Доступна нова версія!\n\n";

      message += "📋 Список змін:</b>\n";
      message += updateInfo[3];

      bot.inlineMenuCallback(message, "🚀 Оновити", "update");
    }
  }
}

void loop() {
  if (WiFiConnected()) {

    if (!botToken.isEmpty()) bot.tick();

    if (validKey) {

      ntp.tick();

      OS.tick();

      // if (!sinricKey.isEmpty()) SinricPro.handle();
    } else {
      if (enabledStrip) {
        for (int i = 100; i >= 1; --i) {
          strip.setBrightness(i * 2.55);
          strip.show();
          delay(10);
        }
        for (int i = 1; i <= 100; i++) {
          strip.setBrightness(i * 2.55);
          strip.show();
          delay(10);
        }
      }
    }
  } else {
    // if (enabledStrip) flagMode();
    // if (displayPresent && enabledDisplay) {
    //   display.clearDisplay();
    //   display.drawBitmap(0, 0 + disy, dino, 28, 32, 1);
    //   display.setTextSize(1);
    //   display.setCursor(32, 0 + disy);    
    //   display.println(utf8cyr("Зв'язок з мережею"));
    //   display.setCursor(32, 8 + disy);
    //   // display.println(utf8cyr("втрачено. Пере-"));
    //   // display.setCursor(32, 16 + disy);
    //   // display.println(utf8cyr("запуск через"));
    //   // display.setCursor(32, 24 + disy);
    //   // display.println(utf8cyr("10 секунд!"));
    //   display.display();
    // }
    // Serial.println("Зв'язок з мережею втрачено. Перезапуск через 10 секунд!");
    // delay(10000);

    Serial.println("Зв'язок з мережею втрачено!");

    ESP.restart();
  }
}