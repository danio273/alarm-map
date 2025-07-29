#include "html.h"

#include <WiFi.h>

#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <AsyncTCP.h>

#include <DNSServer.h>
#include <ESPmDNS.h>

DNSServer dnsServer;

AsyncWebServer server(80);

char* APSsid = "Alarm Map";

// Клас CaptiveRequestHandler для обробки запитів
class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request) {
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", html().c_str());
  }
};

void setupServer() {
  #ifdef DEBUG
    Serial.println("Запуск веб сервера!");
  #endif

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // request->send(200, "text/html", html());
    request->send_P(200, "text/html", html().c_str());
  });

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    for (int i=0;i<params;i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isPost()) {
        #ifdef DEBUG
          Serial.println("З веб сервера отримано " + p->name() + " з значенням " + p->value());
        #endif
        
        // ==================WiFi========================
        if (p->name() == "ssid" && p->value() != ssid) changeMemoryValue("wifi", "ssid", p->value());
        if (p->name() == "pass" && p->value() != password) changeMemoryValue("wifi", "pass", p->value());
        if (p->name() == "wifi-connect-time" && p->value().toInt() != wifiConnectTime / 1000) changeMemoryValue("wifi", "connect_time", long(p->value().toInt() * 1000));
        // ==============================================

        // ================КЛЮЧ==========================
        if (p->name() == "acces-key" && p->value() != accesKey) changeMemoryValue("info", "key", p->value());
        // ==============================================

        // ==============БОТ=============================
        if (p->name() == "token" && p->value() != botToken) changeMemoryValue("bot", "token", p->value());
        if (p->name() == "chat-id" && p->value() != chatID) changeMemoryValue("bot", "chat_id", p->value());
        // ==============================================

        // =================ТРИВОГИ======================
        if (p->name() == "alarms-source" && p->value().toInt() != alarmsSource) changeMemoryValue("alarms", "source", p->value().toInt());
        if (p->name() == "alarms-token" && p->value() != alarmsToken) changeMemoryValue("alarms", "token", p->value());
        if (p->name() == "alarms-interval" && p->value().toInt() != alarmsInterval / 1000) changeMemoryValue("alarms", "interval", long(p->value().toInt() * 1000));
        if (p->name() == "states-order") changeMemoryValue("alarms", "order", p->value().toInt());
        if (p->name() == "green-states") changeMemoryValue("alarms", "green_states", p->value() == "1");
        if (p->name() == "kyiv-mode" && p->value().toInt() != kyivMode)changeMemoryValue("alarms", "kyiv_mode", p->value().toInt());
        if (p->name() == "home-city" && p->value() != homeCity) changeMemoryValue("alarms", "home_city", p->value());
        // ==============================================

        // ===================ЗВУК=======================
        if (p->name() == "sound") {
          if (p->value() == "0") {
            changeMemoryValue("sound", "source", 0);
            soundSource = 0;
          } else soundSource = 1;
        }

        if (soundSource != 0) {  
          if (p->name() == "sound-source") changeMemoryValue("sound", "source", static_cast<int>(p->value().toInt()));
          if (p->name() == "buzzer-pin" && p->value().toInt() != buzzerPin) changeMemoryValue("sound", "pin", static_cast<int>(p->value().toInt()));
          if (p->name() == "buzzer-interval" && p->value().toInt() != buzzerInterval / 1000) changeMemoryValue("sound", "interval", long(p->value().toInt() * 1000));

          if (p->name() == "speaker-volume") changeMemoryValue("sound", "volume", static_cast<int>(p->value().toInt()));
        }
        // ==============================================

        // ==================БУЗЕР=======================
        if (p->name() == "integration") changeMemoryValue("integration", "present", p->value() == "1");
        if (p->name() == "integration-pin") changeMemoryValue("integration", "pin", static_cast<int>(p->value().toInt()));
        if (p->name() == "integration-high") changeMemoryValue("integration", "high", p->value() == "1");
        // ==============================================

        // ===================СТРІЧКА====================
        if (p->name() == "led-pin" && p->value().toInt() != stripPin) changeMemoryValue("led_strip", "pin", static_cast<int>(p->value().toInt()));

        if (p->name() == "led-type" && p->value() != stripType) changeMemoryValue("led_strip", "type", p->value());

        if (p->name() == "silence-minute") changeMemoryValue("led_strip", "silence_minute", p->value() == "1");
        // ==============================================

        // =============АВТОЯСКРАВІСТЬ===================
        if (p->name() == "autobrightness-source") changeMemoryValue("led_strip", "source", static_cast<int>(p->value().toInt()));

        if (p->name() == "day-time") {
          int pos = p->value().indexOf(':');
          changeMemoryValue("led_strip", "day_hour", static_cast<int>(p->value().substring(0, pos).toInt()));
          changeMemoryValue("led_strip", "day_minute", static_cast<int>(p->value().substring(pos + 1).toInt()));
        }
        if (p->name() == "day-brightness" && p->value().toInt() != dayBrightness) changeMemoryValue("led_strip", "day_bright", static_cast<int>(p->value().toInt()));
        if (p->name() == "night-time") {
          int pos = p->value().indexOf(':');
          changeMemoryValue("led_strip", "night_hour", static_cast<int>(p->value().substring(0, pos).toInt()));
          changeMemoryValue("led_strip", "night_minute", static_cast<int>(p->value().substring(pos + 1).toInt()));
        }
        if (p->name() == "night-brightness" && p->value().toInt() != nightBrightness) changeMemoryValue("led_strip", "night_bright", static_cast<int>(p->value().toInt()));

        if (p->name() == "photoresistor-pin") changeMemoryValue("photoresistor", "pin", static_cast<int>(p->value().toInt()));

        if (p->name() == "light-max") changeMemoryValue("photoresistor", "max", static_cast<int>(p->value().toInt()));
        if (p->name() == "light-min") changeMemoryValue("photoresistor", "min", static_cast<int>(p->value().toInt()));
        // ==============================================

        // ==================ДИСПЛЕЙ=====================
        if (p->name() == "display-present") {
          bool newValue = p->value() == "1";
          if (newValue != displayPresent) changeMemoryValue("display", "present", newValue);
        }

        if (p->name() == "display-type") {
          if ((p->value() == "oled-i2c-128x32-ssd1306" || p->value() == "oled-spi-128x32-ssd1306") && 32 != displayHeight) changeMemoryValue("display", "height", 32);
          if ((p->value() == "oled-i2c-128x64-ssd1306" || p->value() == "oled-spi-128x64-ssd1306") && 64 != displayHeight) changeMemoryValue("display", "height", 64);

          if ((p->value() == "oled-i2c-128x32-ssd1306" || p->value() == "oled-spi-128x32-ssd1306" || 
              p->value() == "oled-i2c-128x64-ssd1306" || p->value() == "oled-spi-128x64-ssd1306")
              && 128 != displayWidth) changeMemoryValue("display", "width", 128);

          if ((p->value() == "oled-i2c-128x32-ssd1306" || p->value() == "oled-i2c-128x64-ssd1306") && displayCommunication != "i2c") changeMemoryValue("display", "communication", String("i2c"));
          if ((p->value() == "oled-spi-128x32-ssd1306" || p->value() == "oled-spi-128x64-ssd1306") && displayCommunication != "spi") changeMemoryValue("display", "communication", String("spi"));
        }

        if (p->name() == "flip-display") {
          bool newValue = p->value() == "1";
          if (newValue != displayFlip) changeMemoryValue("display", "flip", newValue);
        }

        if (p->name() == "display-modes") {
          String displayModesString;
          for (int i = 0; i < sizeof(displayModes) / sizeof(displayModes[0]); i++) {
            displayModesString += String(displayModes[i]);
          }

          if (p->value() != displayModesString) changeMemoryValue("display", "modes", p->value());
        }

        if (p->name() == "weather-token" && p->value() != weatherToken) changeMemoryValue("display", "weather_token", p->value());
        if (p->name() == "home-city-id" && p->value().toInt() != homeCityId) changeMemoryValue("display", "city_id", p->value().toInt());

        if (p->name() == "autoswitch-interval" && p->value().toInt() != autoSwitchDisplayInterval / 1000) changeMemoryValue("display", "autoswitchinter", long(p->value().toInt() * 1000));
        // ==============================================

        // =====================DHT======================
        if (p->name() == "temperature-present") {
          bool newValue = p->value() == "1";
          if (newValue != temperaturePresent) changeMemoryValue("temperature", "present", newValue);
        }

        if (p->name() == "temperature-pin" && p->value().toInt() != temperaturePin) changeMemoryValue("temperature", "pin", p->value().toInt());
        if (p->name() == "temperature-type" && p->value().toInt() != temperatureType) changeMemoryValue("temperature", "type", p->value().toInt());
        if (p->name() == "temperature-offset" && p->value().toInt() != temperatureOffset) changeMemoryValue("temperature", "offset", float(p->value().toInt()));
        // ==============================================

        // ===============NTP============================
        if (p->name() == "ntp-host" && p->value() != ntpHost) changeMemoryValue("ntp", "host", p->value());
        if (p->name() == "utc" && p->value().toInt() != timeZone) changeMemoryValue("ntp", "time_zone", p->value().toInt());
        // ==============================================

        // =================SINRIC=======================
        if (p->name() == "sinric" && p->value() == "0" && sinricKey != "") clearNamespace("sinric");

        if (p->name() == "sinric-app-key" && p->value() != sinricKey) changeMemoryValue("sinric", "key", p->value());
        if (p->name() == "sinric-app-secret" && p->value() != sinricSecret) changeMemoryValue("sinric", "secret", p->value());
        if (p->name() == "sinric-led-id" && p->value() != sinricStripId) changeMemoryValue("sinric", "strip_id", p->value());
        if (p->name() == "sinric-temperature-id" && p->value() != sinricTemperatureId) changeMemoryValue("sinric", "temperature_id", p->value());
        // ==============================================
      }
    }
    request->send(200, "text/html", "<html><head><title>Успіх</title><meta charset='UTF-8'></head><body><h1>Збережено! Вашу карту буде перезавантажено для застосування налаштувань!<br><a href=\"/\">Повернутися до налаштувань</a></h1></body></html>");
    delay(1000);
    ESP.restart();
  }); 

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<html><head><title>Успіх</title><meta charset='UTF-8'></head><body><h1>Карту перезавантажено!<br><a href=\"/\">Повернутися до налаштувань</a></h1></body></html>");
    delay(1000);
    ESP.restart();
  });

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    eraseEEPROM();
    request->send(200, "text/html", "<html><head><title>Успіх</title><meta charset='UTF-8'></head><body><h1>Налаштування успішно очищено!<br><a href=\"/\">Повернутися до налаштувань</a></h1></body></html>");
    delay(1000);
    ESP.restart();
  });

  server.on("/light", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(getPhotoresistorValue()).c_str());
  });
}

bool connectWiFi(String ssid, String password) {
  WiFi.mode(WIFI_STA);

  networksCount = WiFi.scanNetworks();

  if (ssid == EMPTY) {
    Serial.println("Налаштування WiFi відустні! ");
    return false;
  }
  
  WiFi.setHostname("Alarm Map");

  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.print("Піключення до " + ssid);
  
  int strips = 0;
  unsigned long previousMillis = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    if (displayPresent) {
      strips = (strips + 1) % 4;
      drawWiFiAnim(strips);
    }
    if (millis() - previousMillis >= wifiConnectTime) return false; 
    delay(500);
  }
  Serial.println("Підключено! IP: " + WiFi.localIP().toString());

  return true; 
}

void initWiFi() {
  Serial.print("Ініціалізація WiFi. ");
  
  if (connectWiFi(ssid, password)) {
    MDNS.begin("alarm-map");
    MDNS.addService("http", "tcp", 80);

    setupServer();
    
    if (!validKey) AsyncElegantOTA.begin(&server);
  
    server.begin();
  } else {
    Serial.print("Не вдалося підключитися! Налаштуйте WiFi за допомогою точки доступу \"" + String(APSsid) + "\"!");

    setupServer();

    WiFi.mode(WIFI_AP);
    WiFi.softAP(APSsid);

    AsyncElegantOTA.begin(&server);

    dnsServer.start(53, "*", WiFi.softAPIP()); 
    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); 

    server.begin();

    if (displayPresent) {
      display.clearDisplay();
      display.drawBitmap(0, 0 + disy, dino, 28, 32, 1);
      display.setTextSize(1);
      displayCenter(WiFi.softAPIP().toString(), 8, 108, 20);
      display.setTextSize(2);
      display.setCursor(20, 16 + disy);
      display.println(APSsid);
      display.display();
    }

    while (true) {
      dnsServer.processNextRequest();
    }
  }
}

void drawWiFiAnim(int index) {
  display.clearDisplay();
  display.drawBitmap(0, 0 + disy, strips[index], 32, 32, 1);
  display.setCursor(35, 8 + disy);
  display.setTextSize(1);
  display.println(utf8cyr("Підключення до:"));
  display.setCursor(35, 16 + disy);
  display.setTextSize(1);
  display.println(ssid);
  display.display();
}

bool WiFiConnected() {return WiFi.status() == WL_CONNECTED;}

void serverEnd() {server.end();}
