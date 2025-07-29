#define OLED_MOSI   22
#define OLED_CLK   18
#define OLED_DC    16
#define OLED_CS    5
#define OLED_RESET 17

int weatherId;
int8_t temp;
uint8_t humidity;

int warMode = -1;
int warInfo[7];

int currencyMode = -1;
float currencyInfo[4];

unsigned long previousUpdateWeatherMode, 
              previousUpdateWarMode, 
              previousUpdateCurrencyMode;

void initDisplay() {
  Serial.println("Ініціалізація дисплею...");

  if (displayCommunication == "i2c") {
    display = Adafruit_SSD1306(displayWidth, displayHeight, &Wire, -1);
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  } 
  if (displayCommunication == "spi") {
    display = Adafruit_SSD1306(displayWidth, displayHeight, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
    display.begin(SSD1306_SWITCHCAPVCC);
  }
  
  disy = (displayHeight - 32) / 2;

  display.clearDisplay();
  display.setTextColor(WHITE);
  if (displayFlip) display.setRotation(2);

  for (int i = 0; i < sizeof(displayModes) / sizeof(displayModes[0]); i++) {
    if (displayModes[i]) modesCount += 1;
  }
  #ifdef DEBUG
    Serial.println("Кількість режимів дисплею: " + String(modesCount));
  #endif
}

void checkAutoSwitchDisplay() {
  int i = -1;
  
  if (displayMode == WAR_MODE) { // Дані про війну
    if (warMode < 6) i = 1;
    else {
      if (!autoSwitchDisplay) i = 1;
      else {warMode = 0; i = 0;}
    }
  } else if (displayMode == CURRENCY_MODE) { // Дані про курс валют
    if (currencyMode < 3) i = 2;
    else {
      if (!autoSwitchDisplay) i = 2;
      else {currencyMode = 0; i = 0;}
    }
  } else if (autoSwitchDisplay) i = 0;

  switch (i) {
    case 1: warMode = changeMode(warMode, 6); break;
    case 2: currencyMode = changeMode(currencyMode, 3); break;
    case 0: nextDisplayMode(); break;
    default: break;
  }

  dsiplayInfo();
}

int changeMode(int currentMode, int maxMode) {
  if (currentMode < maxMode) {
    return currentMode + 1;
  } else {
    return 0;
  }
}

void dsiplayInfo() {
  switch (displayMode) {
    case TIME_MODE: displayTime(); break;
    case DATE_MODE: displayDate(); break;
    case CONDITIONS_MODE: displayConditions(); break;
    case WEATHER_MODE: displayWeather(); break;
    case ALARM_INFO_MODE: displayAlarmInfo(); break;
    case WAR_MODE: displayWar(); break;
    case CURRENCY_MODE: displayCurrency(); break;
    default: break;
  }
}
void displayTime() {
  int hour = ntp.hour();
  int minute = ntp.minute();

  String time;
  if (hour < 10) time += "0";
  time += hour;
  if (ntp.second() % 2 == 0) {
    time += ":";
  } else {
    time += " ";
  }
  if (minute < 10) time += "0";
  time += minute;

  display.setCursor(0, 0 + disy);
  display.clearDisplay();
  display.setTextSize(4); 
  displayCenter(time, 0, 132, 0);
}
void displayDate() {
  String date[2];
  
  getDate(date);

  display.setTextSize(2); 
  display.clearDisplay();
  displayCenter(utf8cyr(date[0]), 0, 132, 0);
  displayCenter(date[1], 16, 132, 0);
}
void displayConditions() {
  float temperature = getTemperature();
  float humidity = getHumidity();

  display.clearDisplay();
  display.drawBitmap(0, 0 + disy, emojis[checkConditions(temperature,humidity)], 32, 32, 1);
  String weather = "";
  weather += int(temperature);
  weather += "C ";
  weather += int(humidity);
  weather += "%";
  display.setTextSize(2);
  displayCenter(weather, 8, 100, 32);
}
void displayWeather() {
  if (millis() - previousUpdateWeatherMode >= 1200000 || previousUpdateWeatherMode == 0) { //Інтервал 20 хвилин
    getWeatherData();
    previousUpdateWeatherMode = millis();
  }

  display.clearDisplay();
  display.drawBitmap(0, 0 + disy, weatherImages[weatherId], 32, 32, 1);
  String weather = EMPTY;
  weather += temp;
  weather += "C ";
  weather += humidity;
  weather += "%";
  display.setTextSize(2);
  displayCenter(weather, 8, 100, 32);
}
void displayAlarmInfo() {
  unsigned long minutes = (duration / 60000) % 60;
  unsigned long hours = (duration / 3600000) % 24;

  display.clearDisplay();
  display.drawBitmap(0, 0 + disy, alarmIcons[!homeCityEnable ? 0 : 1], 32, 32, 1);
  display.setTextSize(3);

  String durationTransformed = EMPTY;
  if (hours < 10) durationTransformed += "0";
  durationTransformed += hours;
  durationTransformed += ":";
  if (minutes < 10) durationTransformed += "0";
  durationTransformed += minutes;

  displayCenter(durationTransformed, 4, 100, 32);
  display.display();
}
void displayWar(){
  if (millis() - previousUpdateWarMode >= 600000 || previousUpdateWarMode == 0) {
    updateWarData();
    previousUpdateWarMode = millis();
  }

  display.clearDisplay();
  display.drawBitmap(0, 0 + disy, warIcons[warMode], 32, 32, 1);
  display.setTextSize(3);
  displayCenter(String(numberToFormattedString(warInfo[warMode])), 4, 100, 32);
  display.display();
}
void displayCurrency() {
  if (millis() - previousUpdateCurrencyMode >= 600000 || previousUpdateCurrencyMode == 0) {
    getCurrencyData();
    previousUpdateCurrencyMode = millis();
  }

  display.clearDisplay();
  display.setTextSize(3);
  displayCenter(String(currencyInfo[currencyMode]), 4, 100, 32);
  display.drawBitmap(0, 0 + disy, currencyIcons[currencyMode], 32, 32, 1);
  display.display();
} 
void getDate(String date[]) {
  String daysOfWeek[] = {"Понеділок", "Вівторок", "Середа", "Четвер", "П'ятниця", "Субота", "Неділя"};
  date[0] = daysOfWeek[ntp.dayWeek() - 1];

  uint8_t day = ntp.day();
  uint8_t month = ntp.month();
  uint16_t year = ntp.year();

  if (day < 10) date[1] += "0";
  date[1] += day;
  date[1] += ".";
  if (month < 10) date[1] += "0";
  date[1] += month;
  date[1] += ".";
  date[1] += year;
}
void getWeatherData() {
  String payload;
  String url = "https://api.openweathermap.org/data/2.5/weather?id=" + String(homeCityId) + "&units=metric&appid=" + weatherToken;
  if (httpRequest(url.c_str(), payload)) {
    StaticJsonDocument<512> doc;
    deserializeJson(doc, payload);

    weatherId = doc["weather"][0]["id"].as<int>();
    temp = doc["main"]["temp"].as<int8_t>();
    humidity = doc["main"]["humidity"].as<uint8_t>();

    if (weatherId <= 800) weatherId = weatherId / 100;
    if (weatherId > 800) weatherId = 9;
    if(weatherId > 4) weatherId -= 3;
    else weatherId -= 2;
    if(weatherId == 2) weatherId--;
  }
}
void updateWarData() {
  String payload;

  if (httpRequest("https://russianwarship.rip/api/v2/statistics/latest", payload)) {
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, payload);

    warInfo[0] = jsonDoc["data"]["day"].as<int>();
    warInfo[1] = jsonDoc["data"]["stats"]["personnel_units"].as<int>();
    warInfo[2] = jsonDoc["data"]["stats"]["tanks"].as<int>();
    warInfo[3] = jsonDoc["data"]["stats"]["armoured_fighting_vehicles"].as<int>();
    warInfo[4] = jsonDoc["data"]["stats"]["artillery_systems"].as<int>();
    warInfo[5] = jsonDoc["data"]["stats"]["cruise_missiles"].as<int>();
    warInfo[6] = jsonDoc["data"]["stats"]["uav_systems"].as<int>();
  }
}
void getCurrencyData() {
  String payload;

  if (httpRequest("https://api.privatbank.ua/p24api/pubinfo?json&exchange&coursid=5", payload)) {
    StaticJsonDocument<512> doc;
    deserializeJson(doc, payload);

    JsonArray data = doc.as<JsonArray>();

    for (JsonObject currency : data) {
      String ccy = currency["ccy"];
      String base_ccy = currency["base_ccy"];
      String buy = currency["buy"];
      String sale = currency["sale"];

      if (ccy == "USD") {
        currencyInfo[0] = buy.substring(0, buy.indexOf('.') + 3).toFloat();
        currencyInfo[1] = sale.substring(0, sale.indexOf('.') + 3).toFloat();
      } else if (ccy == "EUR") {
        currencyInfo[2] = buy.substring(0, buy.indexOf('.') + 3).toFloat();
        currencyInfo[3] = sale.substring(0, sale.indexOf('.') + 3).toFloat();
      }
    }
  }
}

void displayCenter(String text, int y, int screenWidth, int offset) {
  int16_t x1, y1;
  uint16_t width, height;

  display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
  display.setCursor(((screenWidth - width) / 2) + offset, y + disy);
  display.println(text); // text to display
  display.display();
}

void turnOffDisplay() {
  if (enabledDisplay) {
    enabledDisplay = false;
    changeMemoryValue("display", "enabled", enabledDisplay);
  }
  display.clearDisplay();
  display.display();
}

void turnOnDisplay() {
  if (!enabledDisplay) {
    enabledDisplay = true;
    changeMemoryValue("display", "enabled", enabledDisplay);
  }
}

void nextDisplayMode() {
  if (displayMode != sizeof(displayModes) / sizeof(displayModes[0])) displayMode += 1;
  else displayMode = 0;

  while (!displayModes[displayMode]) {
    if (displayMode != sizeof(displayModes) / sizeof(displayModes[0])) displayMode += 1;
    else displayMode = 0;
  }
}

String numberToFormattedString(int number) {
  if (number >= 10000) {
    int k = number / 1000;
    String result = String(k);
    number %= 1000;
    if (k < 100) {
        result += '.';
        result += number / 100;  // Змінено на number / 100, щоб вивести тільки одну цифру після крапки
    }
    result += 'K';
    return result;
  } else return String(number);
}

String utf8cyr(String source) {
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;
  while (i < k) {
    n = source[i]; i++;
    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {                                // перекодировать 0 блок (прописные)
          n = source[i]; i++;                 
          if (n == 0x81) { n = 0xA8; break; }       // перекодировать букву Ё
          if (n == 0x84) { n = 0xAA; break; }       // перекодировать букву Є
          if (n == 0x86) { n = 0xB1; break; }       // перекодировать букву І                                                        
          if (n == 0x87) { n = 0xAF; break; }       // перекодировать букву Ї
          if (n >= 0x90 && n <= 0xBF) n = n + 0x2F; break; // перекодировать остальные буквы 0 блока
        }
        case 0xD1: {                                // перекодировать 1 блок (строчные)                   
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB7; break; }       // перекодировать букву ё 
          if (n == 0x94) { n = 0xB9; break; }       // перекодировать букву є
          if (n == 0x96) { n = 0xB2; break; }       // перекодировать букву і             
          if (n == 0x97) { n = 0xBE; break; }       // перекодировать букву ї 
          if (n >= 0x80 && n <= 0x8F) n = n + 0x6F; break; // перекодировать остальные буквы 1 блока
        }
        case 0xD2: {                                // перекодировать 2 блок (всё вместе)
          n = source[i]; i++;
          if (n == 0x90) { n = 0xA5; break; }       // перекодировать букву Ґ          
          if (n == 0x91) { n = 0xB3; break; }       // перекодировать букву ґ             
        }
      }
    }
    m[0] = n; 
    target = target + String(m);
  }
  return target;
}
