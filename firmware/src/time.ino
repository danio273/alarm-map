void initTime() {
  Serial.println("Ініціалізація NTP...");
  ntp.setHost(ntpHost.c_str());
  ntp.setGMT(timeZone);
  ntp.begin();
  
  ntp.updateNow();

  while (!checkTime(ntp.unix())) ntp.updateNow();
}

bool checkTime(uint32_t unix) {
  if (unix > 662688000 && unix < 2051222400) {
    #ifdef DEBUG
      Serial.println("Час правильний!");
    #endif
    return true;
  } else {
    #ifdef DEBUG
      Serial.println("Помилка! Не вдалося отримати правильний час!");
    #endif
    return false;
  }
}