bool ledColorBlue[] = {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
                    // 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26

static unsigned long times[STATES_COUNT];
static int ledColor[STATES_COUNT];

void initStrip() {
  Serial.println("Ініціалізація стрічки...");

  int kyivStatePlace, crimeaStatePlace;
  if (statesOrder == 0) {
    kyivStatePlace = 7;
    crimeaStatePlace = 15;
  } else if (statesOrder == 1) {
    const char* newStates[] = {"Одеська область", "Миколаївська область", "Дніпропетровська область", "Полтавська область", "Черкаська область", "Кіровоградська область", "Вінницька область", "Хмельницька область", "Чернівецька область", "Закарпатська область", "Івано-Франківська область", "Тернопільська область", "Львівська область", "Волинська область", "Рівненська область", "Житомирська область", "Київська область", "Чернігівська область", "Сумська область", "Харківська область", "Луганська область", "Донецька область", "Запорізька область", "Херсонська область", "АР Крим", "м. Київ"};
    for (int i = 0; i < sizeof(states) / sizeof(states[0]); ++i) {
        states[i] = newStates[i];
    }
    int newLedColorBlue[] = {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
                          // 1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26
    for (int i = 0; i < sizeof(ledColorBlue) / sizeof(ledColorBlue[0]); ++i) {
        ledColorBlue[i] = newLedColorBlue[i];
    }
    kyivStatePlace = 16;
    crimeaStatePlace = 24;
  }

  switch (kyivMode) {
    case 0:
      statesCount = 25;
      states[kyivStatePlace] = "Київська область";
      if (alarmsSource == 2) states[crimeaStatePlace] = "Автономна Республіка Крим";
      break;
    case 1:
      statesCount = 25;
      states[kyivStatePlace] = "м. Київ";
      if (alarmsSource == 2) states[crimeaStatePlace] = "Автономна Республіка Крим";
      break;
    case 2:
      statesCount = 26;
      for (int i = statesCount - 1; i > kyivStatePlace + 1; i--) { 
        states[i] = states[i - 1];
        ledColorBlue[i] = ledColorBlue[i - 1];
      }
      states[kyivStatePlace + 1] = "м. Київ";
      ledColorBlue[kyivStatePlace + 1] = true;
      if (alarmsSource == 2) states[crimeaStatePlace + 1] = "Автономна Республіка Крим";
      break;
  }

  if (stripType == "grb") strip.updateType(NEO_GRB + NEO_KHZ800);
  else if (stripType == "rgb") strip.updateType(NEO_RGB + NEO_KHZ800);
  else if (stripType == "rbg") strip.updateType(NEO_RBG + NEO_KHZ800);
  else if (stripType == "gbr") strip.updateType(NEO_GBR + NEO_KHZ800);
  else if (stripType == "brg") strip.updateType(NEO_BRG + NEO_KHZ800);
  else if (stripType == "bgr") strip.updateType(NEO_BGR + NEO_KHZ800);

  strip.updateLength(statesCount);
  strip.setPin(stripPin);

  strip.begin();
  strip.show();
  changeBrightness(brightness, autoBrightness);

  if (enabledStrip) flagMode();
}

void displayStrip() {
  #ifdef DEBUG
    Serial.println("Оновлення стрічки!");
  #endif
  if (enabledStrip) {
    if (!(silenceMinuteEnabled && ntp.hour() == 9 && ntp.minute() < 1)) {
      switch (mode) {
        case 0: alarmsMode(enabledStates); break;
        case 1: flagMode(); break;
        case 2: 
          {
            if (color == "white") flashlightMode(strip.Color(255, 255, 255));
            if (color == "red") flashlightMode(strip.Color(255, 0, 0));
            if (color == "orange") flashlightMode(strip.Color(255, 165, 0));
            if (color == "yellow") flashlightMode(strip.Color(255, 255, 0));
            if (color == "green") flashlightMode(strip.Color(0, 255, 0)); 
            if (color == "blue") flashlightMode(strip.Color(0, 0, 255));
            if (color == "purple") flashlightMode(strip.Color(128, 0, 128)); 
          }
          break;
        case 3: alarmsMode(enabledStates); break;
      }
    } else flagMode();
  } else turnOffStrip();
}

void alarmsMode(bool enabledStates[STATES_COUNT]) {
  unsigned long hv = 180000;

  for (int i = 0; i < statesCount; i++) {
    bool alarmEnable = enabledStates[i];

    //Тривога активна і тривалість тривоги = 0
    if (alarmEnable && times[i] == 0) {
      times[i] = millis(); //Встановлюється час початку тривоги
      ledColor[i] = 2; //Помаранчевий колір
    } 
    //Тривога активна & час початку тривоги + інтервал > теперешній час & тривога не триває довше 3 хвилин
    else if (alarmEnable && times[i] + hv > millis() && ledColor[i] != 1) {
      ledColor[i] = 2; //Помаранчевий колір
    } 
    //Тривога активна (якщо тривога тривває довше 3 хвилин)
    else if (alarmEnable) {
      ledColor[i] = 1; //Червоний колір
      times[i] = millis();
    }

    //Тривога не активна & час від відбою тривоги + інтервал > теперешній час &
    if (!alarmEnable && times[i] + hv > millis() && times[i] != 0) {
      ledColor[i] = 3; //Зелений колір
    }
    //Тривога не активна 
    else if (!alarmEnable) {
      ledColor[i] = 0;
      times[i] = 0;
    }    
  }

  if (mode == 3) strip.clear();

  for (int i = 0; i < statesCount; i++) {
    if (mode != 3 || states[i] == homeCity || (kyivMode == 0 && homeCity == "м. Київ" && states[i] == "Київська область")) {
      switch (ledColor[i]) {
        //Червоний колір - відображається якщо тривога триває довше 3 хвилин
        case 1: strip.setPixelColor(i, strip.Color(255,0,0)); break;
        //Помаранчевий колір - відображається протягом 3 хвилин після початку тривоги
        case 2: strip.setPixelColor(i, strip.Color(255,55,0)); break;
        //В залежності від налаштувань після трьох хвилин після закінчення тривоги лед вимикається або і далі світить зеленим кольром
        case 0: if (greenStates) {} else {strip.setPixelColor(i, strip.Color(0, 0, 0)); break;}
        //Зелений колір - відображається протягом 3 хвилин після закінчення тривоги
        case 3: strip.setPixelColor(i, strip.Color(0,255,0)); break;
      }
    }
  }
  strip.show();
}

void flagMode() {
  strip.clear();
  for (int i = 0; i < statesCount; i++) strip.setPixelColor(i, ledColorBlue[i] ? strip.Color(0,87,184) : strip.Color(255,213,0));
  strip.show();
}

void flashlightMode(uint32_t color) { //Режим ліхтарика
  for (int i = 0; i < statesCount; i++) strip.setPixelColor(i, color);
  strip.show();
} 

void turnOffStrip() {
  if (enabledStrip) {
    enabledStrip = false;

    // if (!sinricStripId.isEmpty()) {
    //   SinricProDimSwitch &sinricStrip = SinricPro[sinricStripId];
    //   sinricStrip.sendPowerStateEvent(enabledStrip);
    // }

    // if (!sinricTemperatureId.isEmpty()) {
    //   SinricProTemperaturesensor &sinricTemperature = SinricPro[sinricTemperatureId];
    //   sinricTemperature.sendPowerStateEvent(enabledStrip);
    // }

    changeMemoryValue("led_strip", "enabled", enabledStrip);
  }
  
  strip.clear();
  strip.show();
}

void turnOnStrip() {
  if (!enabledStrip) {
    enabledStrip = true;

    // if (!sinricStripId.isEmpty()) {
    //   SinricProDimSwitch &sinricStrip = SinricPro[sinricStripId];
    //   sinricStrip.sendPowerStateEvent(enabledStrip);
    // }

    // if (!sinricTemperatureId.isEmpty()) {
    //   SinricProTemperaturesensor &sinricTemperature = SinricPro[sinricTemperatureId];
    //   sinricTemperature.sendPowerStateEvent(enabledStrip);
    // }

    changeMemoryValue("led_strip", "enabled", enabledStrip);
  }
}

void changeBrightness(int value, bool autoBrightnessValue) { //Зміна яскравості    
  #ifdef DEBUG
    Serial.println("Яскравість встановлено на " + String(value) + "%, автоясркавість " + String(autoBrightnessValue));
  #endif
  
  if (value != brightness) {
    brightness = value;

    // if (!sinricStripId.isEmpty()) {
    //   SinricProDimSwitch &sinricStrip = SinricPro[sinricStripId];
    //   sinricStrip.sendPowerLevelEvent(brightness);
    // }

    changeMemoryValue("led_strip", "brightness", brightness);
  }

  if (autoBrightnessValue != autoBrightness) {
    autoBrightness = autoBrightnessValue;
    changeMemoryValue("led_strip", "auto_brightness", autoBrightness);
  }

  strip.setBrightness(value * 2.55);
  strip.show();
}

void timeAutoBrightness() {
  #ifdef DEBUG
    Serial.println("Перевірка автояскравості на підставі часу...");
  #endif

  bool isDay;
  int hour = ntp.hour();
  int minute = ntp.minute();

  if (((nightHour > dayHour && (hour >= nightHour || hour < dayHour)) || (nightHour <= dayHour && (hour <= nightHour || hour < dayHour))) ||
      (hour == nightHour && minute >= nightMinute) ||
      (hour == dayHour && minute < dayMinute)) 
    isDay = false;
  else isDay = true;

  if (mode != 2) changeBrightness(isDay ? dayBrightness : nightBrightness, true);
  else changeBrightness(100, true);
}

void sensorAutoBrightness() {
  #ifdef DEBUG
    Serial.println("Перевірка автояскравості на підставі фоторезистора...");
  #endif

  if (mode != 2) changeBrightness(getPhotoresistorProcent(), true);
  else changeBrightness(100, true);
}