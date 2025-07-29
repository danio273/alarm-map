#include <HTTPClient.h>
#include <HTTPUpdate.h>

bool httpRequest(const char* url, String& payload) {
  #ifdef DEBUG
    Serial.println("Запит на " + String(url));
  #endif

  HTTPClient http;

  http.begin(url);

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    #ifdef DEBUG
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    #endif
    payload = http.getString();
    #ifdef DEBUG
      Serial.println(payload);
    #endif
    http.end();
    return true;
  } else {
    #ifdef DEBUG
      Serial.print("Помилка: ");
      Serial.println(httpResponseCode);
    #endif
    http.end();
    return false;
  }
}

void checkAlarms() {
  #ifdef DEBUG
    Serial.println("Перевірка тривог...");
  #endif

  bool success = false;

  String response;
  switch (alarmsSource) {
    case 0: success = httpRequest("https://vadimklimenko.com/map/statuses.json", response); break;
    case 1: success = httpRequest("https://ubilling.net.ua/aerialalerts/", response); break;
    case 2: 
      {
        #ifdef DEBUG
          Serial.println("Запит на https://api.ukrainealarm.com/api/v3/alerts/");
        #endif

        HTTPClient http;
        http.begin("https://api.ukrainealarm.com/api/v3/alerts/");
        http.addHeader("Authorization", alarmsToken);

        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
          #ifdef DEBUG
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
          #endif
          response = http.getString();
          #ifdef DEBUG
            Serial.println(response);
          #endif
          http.end();
          success = true;
        } else {
          #ifdef DEBUG
            Serial.print("Помилка: ");
            Serial.println(httpResponseCode);
          #endif
          http.end();
          success = false;
        }
      }
      break;
    case 3: success = httpRequest("http://alerts.net.ua/alerts_statuses_v3.json", response); break;
  }

  if (success) {
    DynamicJsonDocument doc(30000);

    deserializeJson(doc, response);

    for (int i = 0; i < STATES_COUNT; i++) {
      bool alarmEnabled = false;
      if (alarmsSource == 0) {
        alarmEnabled = doc["states"][states[i]]["enabled"].as<bool>();
      } else if (alarmsSource == 1) {
        alarmEnabled = doc["states"][states[i]]["alertnow"].as<bool>();
        if (states[i] == "АР Крим") alarmEnabled = true;
      } else if (alarmsSource == 2) {
        for (int n = 0; n < doc.size(); n++) {
          if (doc[n]["regionName"] == states[i]) {
            alarmEnabled = true;
            break;
          }
        }
      } else if (alarmsSource == 3) {
        alarmEnabled = doc["states"][states[i]].as<bool>();
        if (states[i] == "АР Крим") alarmEnabled = true;
      }
      enabledStates[i] = alarmEnabled ? true : false;

      if (states[i] == homeCity) {
        homeCityEnable = alarmEnabled;
        if (lastEnable != homeCityEnable || startTime == 0) {
          startTime = millis();
          lastEnable = homeCityEnable;

          if (homeCityEnable && autoSwitch && mode != 0 && mode != 3) mode = 0;

          if (!alarmFirstUpdate) {
            if (soundSource == 1) buzzerAlarmSound(homeCityEnable);
            if (soundSource == 2) speakerAlarmSound(homeCityEnable);
          }

          if (integrationPresent) digitalWrite(integrationPin, (homeCityEnable ? (integrationHigh ? HIGH : LOW) : (integrationHigh ? LOW : HIGH)));
        }
        duration = millis() - startTime;
      }
    }

    alarmFirstUpdate = false;
  } else {
    checkAlarms();
  }
}