#include <Preferences.h>
Preferences pref;

void initEEPROM() {
  Serial.println("Ініціалізація EEPROM...");

  pref.begin("info" , true);
  versionNumber = pref.getInt("version_number", 10);
  version = pref.getString("version", "1.3.1");
  accesKey = pref.getString("key", EMPTY);
  pref.end();

  pref.begin("wifi", true);
  ssid = pref.getString("ssid", EMPTY);
  password = pref.getString("pass", EMPTY);
  wifiConnectTime = pref.getLong("connect_time", 10000);
  pref.end();

  pref.begin("bot", true);
  botToken = pref.getString("token", EMPTY);
  chatID = pref.getString("chat_id", EMPTY);
  pref.end();

  pref.begin("alarms", true);
  alarmsSource = pref.getInt("source", 0);
  alarmsToken = pref.getString("token", EMPTY);
  alarmsInterval = pref.getLong("interval", 10000);

  statesOrder = pref.getInt("order", 0);
  greenStates = pref.getBool("green_states", true);
  kyivMode = pref.getInt("kyiv_mode", 0);

  homeCity = pref.getString("home_city", EMPTY);

  autoSwitch = pref.getBool("auto_switch", false);
  pref.end();

  pref.begin("sound", true);
  soundSource = pref.getInt("source", 0);

  buzzerPin = pref.getInt("pin", 0);
  buzzerInterval = pref.getLong("interval", 15000);

  speakerVolume = pref.getInt("volume", 50);
  pref.end();

  pref.begin("integration", true);
  integrationPresent = pref.getBool("present", false);
  integrationPin = pref.getInt("pin", 0);
  integrationHigh = pref.getBool("high", true);
  pref.end();

  pref.begin("led_strip", true);
  stripPin = pref.getInt("pin", 13);

  stripType = pref.getString("type", "grb");

  enabledStrip = pref.getBool("enabled", true);

  brightness = pref.getInt("brightness", 50);

  autoBrightnessSource = pref.getInt("source", 0);
  autoBrightness = pref.getBool("auto_brightness", false);

  dayHour = pref.getInt("day_hour", 0);
  dayMinute = pref.getInt("day_minute", 0);
  nightHour = pref.getInt("night_hour", 0);
  nightMinute = pref.getInt("night_minute", 0);
  dayBrightness = pref.getInt("day_bright", 50);
  nightBrightness = pref.getInt("night_bright", 10);

  silenceMinuteEnabled = pref.getBool("silence_minute", false);
  pref.end();

  pref.begin("photoresistor", true);
  photoresistorPin = pref.getInt("pin", 0);
  lightMax = pref.getInt("max", -1);
  lightMin = pref.getInt("min", -1);
  pref.end();

  pref.begin("display", true);
  displayPresent = pref.getBool("present", true);

  displayWidth = pref.getInt("width", 128);
  displayHeight = pref.getInt("height", 32);
  displayCommunication = pref.getString("communication", "i2c");

  enabledDisplay = pref.getBool("enabled", true);

  displayFlip = pref.getBool("flip", false);

  String displayModesString = pref.getString("modes", "0000000");
  for (int i = 0; i < sizeof(displayModes) / sizeof(displayModes[0]); i++) {
    if (i < displayModesString.length()) {
      displayModes[i] = (displayModesString.charAt(i) == '1');
    } else {
      displayModes[i] = false;
    }
  }

  autoSwitchDisplayInterval = pref.getLong("autoswitchinter", 5000);

  weatherToken = pref.getString("weather_token", EMPTY);
  homeCityId = pref.getInt("city_id", 0);

  autoSwitchDisplay = pref.getBool("auto_switch", true);
  if (!autoSwitchDisplay) {
    displayMode = pref.getInt("mode", 0);
  }
  pref.end();

  pref.begin("temperature", true);
  temperaturePresent = pref.getBool("present", false);
  temperaturePin = pref.getInt("pin", 0);
  temperatureType = pref.getInt("type", 11);
  temperatureOffset = pref.getFloat("offset", 0.0);
  pref.end();

  pref.begin("ntp", true);
  ntpHost = pref.getString("host", "ua.pool.ntp.org");
  timeZone = pref.getInt("time_zone", 0);
  pref.end();

  pref.begin("sinric", true);
  sinricKey = pref.getString("key", EMPTY);
  sinricSecret = pref.getString("secret", EMPTY);
  sinricStripId = pref.getString("strip_id", EMPTY);
  sinricTemperatureId = pref.getString("temperature_id", EMPTY);
  pref.end();
}

void changeMemoryValue(const char* name, const char* key, String value) {
  pref.begin(name);
  if (pref.getString(key, EMPTY) != value) {
    pref.putString(key, value);

    #ifdef DEBUG
      Serial.println("Змінній " + String(key) + " в " + String(name) + " встановлено значення " + value + " перевірка новго значення дала " + pref.getString(key, EMPTY));
    #endif
  }
  pref.end();
}

void changeMemoryValue(const char* name, const char* key, int value) {
  pref.begin(name);
  if (pref.getInt(key, 0) != value) {
    pref.putInt(key, value);

    #ifdef DEBUG
      Serial.println("Змінній " + String(key) + " в " + String(name) + " встановлено значення " + String(value) + ", перевірка новго значення дала " + String(pref.getInt(key, 0)));
    #endif
  }
  pref.end();
}

// ⬇️ ХУЙНЯ ЯКАСЬ ⬇️
// int getMemoryValue(const char* name, const char* key, int defaultValue) {
//   pref.begin(name);
//   int value = pref.getBool(key, defaultValue);
//   pref.end();

//   #ifdef DEBUG
//     Serial.println("Отримано значення " + String(result) + " з " + String(key) + " в " + String(name));
//   #endif

//   return value;
// }
// ⬆️⬆️⬆️⬆️⬆️⬆️⬆️

void changeMemoryValue(const char* name, const char* key, bool value) {
  pref.begin(name);
  if (pref.getBool(key, false) != value) {
    pref.putBool(key, value);

    #ifdef DEBUG
      Serial.println("Змінній " + String(key) + " в " + String(name) + " встановлено значення " + String(value) + " перевірка новго значення дала " + String(pref.getBool(key, 0)));
    #endif
  }
  pref.end();
}

void changeMemoryValue(const char* name, const char* key, long value) {
  pref.begin(name);
  if (pref.getLong(key, 0) != value) {
    pref.putLong(key, value);

    #ifdef DEBUG
      Serial.println("Змінній " + String(key) + " в " + String(name) + " встановлено значення " + String(value) + ", перевірка новго значення дала " + String(pref.getLong(key, 0)));
    #endif
  }
  pref.end();
}

void changeMemoryValue(const char* name, const char* key, float value) {
  pref.begin(name);
  if (pref.getFloat(key, 0.0) != value) {
    pref.putFloat(key, value);

    #ifdef DEBUG
      Serial.println("Змінній " + String(key) + " в " + String(name) + " встановлено значення " + String(value) + ", перевірка новго значення дала " + String(pref.getFloat(key, 0.0)));
    #endif
  }
  pref.end();
}

void eraseEEPROM() {
  clearNamespace("info");
  clearNamespace("wifi");
  clearNamespace("bot");
  clearNamespace("alarms");
  clearNamespace("led_strip");
  clearNamespace("display");
  clearNamespace("ntp");
  clearNamespace("sinric");
}

void clearNamespace(const char* name) {
  pref.begin(name);
  pref.clear();
  pref.end();

  #ifdef DEBUG
    Serial.println("Очищено " + String(name));
  #endif
}