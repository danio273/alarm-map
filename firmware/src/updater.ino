int lastProcent;

void checkUpdates(String updates[]) {
  String response;

  if (httpRequest("https://alarm-map.sdl.pp.ua/firmware/premium/firmware/version.json", response)) {
    DynamicJsonDocument doc(5000);
    deserializeJson(doc, response);

    updates[0] = doc["number"].as<String>();
    updates[1] = doc["version"].as<String>();
    updates[2] = doc["url"].as<String>();
    updates[3] = doc["description"].as<String>();
  }
}

void updateFirmware(int message_id) {
  bot.tickManual();

  if (updateInfo[0].toInt() > versionNumber) {
    httpUpdate.onStart([message_id]() {
      bot.editMessage(message_id, "🔧 Завантаження прошивки...");
    });
    httpUpdate.onEnd([message_id, updateInfo]() {
      changeMemoryValue("info", "version_number", static_cast<int>(updateInfo[0].toInt()));
      changeMemoryValue("info", "version", updateInfo[1]);

      bot.editMessage(message_id, "✅ Оновлення успішне");
      bot.sendMessage("🔄 Перезапуск...");
    });
    httpUpdate.onProgress([message_id](int progress, int total) {
      int procent = (static_cast<double>(progress) / total) * 100;
      if (procent != lastProcent) {
        lastProcent = procent;

        if (displayPresent && enabledDisplay) {
          display.clearDisplay();
          display.drawBitmap(0, 0 + disy, update, 32, 32, 1);
          display.setTextSize(1);
          displayCenter(utf8cyr("Оновлення..."), 4, 100, 32);
          displayCenter(utf8cyr(String(procent) + "%"), 16, 100, 32);
          display.display();
        }
      }
    });
    httpUpdate.onError([message_id](int err) {
      bot.editMessage(message_id, "❌ Помилка оновлення - " + String(err) + "!");
    });

    WiFiClient client;

    t_httpUpdate_return ret = httpUpdate.update(client, updateInfo[2]);

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        #ifdef DEBUG
          Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        #endif
        break;

      case HTTP_UPDATE_NO_UPDATES:
        #ifdef DEBUG
          Serial.println("HTTP_UPDATE_NO_UPDATES");
        #endif
        break;

      case HTTP_UPDATE_OK:
        #ifdef DEBUG
          Serial.println("HTTP_UPDATE_OK");
        #endif
        break;
    }
  } else {
    bot.editMessage(message_id, "❌ Оновлення відсутні");
  }
}