void initBot() {
  Serial.println("Ініціалізація бота...");

  bot.setToken(botToken);
  bot.setChatID(chatID);
 
  bot.setTextMode(FB_HTML);

  bot.skipUpdates(); // Ігнорувати попередні повідомлення

  bot.attach(newMsg);

  bot.showMenuText("💡", replyKeyboard(), chatID);

  if (!validKey) bot.sendMessage("❗️ Пристрій заблоковано! " + checkKeyMessage + " За необхідності зверніться в службу підтримки @alarm_map_support.");
}

void newMsg(FB_msg& msg) {
  #ifdef DEBUG
    Serial.println(msg.toString());
  #endif

  String chat_id = msg.chatID;
  String text = msg.text;
  String data = msg.data;
  int message_id = msg.messageID;
  bool query = msg.query;
  
  if (text == "/start") {
    bot.showMenuText("👋", replyKeyboard());
  }

  else if (text == "⏯💡") {
    if (enabledStrip) {
      turnOffStrip();
      bot.sendMessage("⏸💡");
    } else {
      turnOnStrip();
      bot.sendMessage("▶️💡");
    }
  }

  else if (text == "⏯📺") {
    if (enabledDisplay) {
      turnOffDisplay();
      bot.sendMessage("⏸📺");
    } else {
      turnOnDisplay();
      bot.sendMessage("▶️📺");
    }
  }

  else if (text == "🔆 Змінити яскравість" ||
      data == "100" || data == "75" || data == "50" || data == "25" || data == "1" ||
      data == "enable_auto_brightness") {

    if (data == "100" || data == "75" || data == "50" || data == "25" || data == "1") {
      autoBrightness = false;
      if (data.toInt() != brightness) {
        changeBrightness(data.toInt(), false);
      }
    }

    if (data == "enable_auto_brightness") {
      autoBrightness = true;
      changeMemoryValue("led_strip", "auto_brightness", autoBrightness);
    }

    String keyboard = "100%\t 75%\t 50%\t 25%\t 1%";
    String callback = "100, 75, 50, 25, 1";
    if (!autoBrightness) {
      keyboard += "\n🤖 Активувати автояскравість";
      callback += ",enable_auto_brightness";
    }

    if (!query) {
      bot.inlineMenuCallback("🔆 Обери значення:", keyboard, callback);
    } else {
      bot.editMessage(message_id, "🔆 Обери значення:");
      bot.editMenuCallback(message_id, keyboard, callback);
    }
  } 
  
  else if ((text == "🔢💡" || data == "choose_mode" ||
      data == "alarms_mode" || data == "flag_mode" || data == "flashlight_mode" || data == "alarm_mode" ||
      data == "white" || data == "red" || data == "orange" || data == "yellow" || data == "green" || data == "blue" || data == "purple" ||
      data == "enable_auto_switch" || data == "disable_auto_switch") && 
      validKey) {

    if (data == "alarms_mode") mode = 0;
    if (data == "flag_mode") mode = 1;
    if (data == "alarm_mode") mode = 3;
    
    if (data == "enable_auto_switch" || data == "disable_auto_switch") {
      if (data == "enable_auto_switch") autoSwitch = true;
      if (data == "disable_auto_switch") autoSwitch = false;
      changeMemoryValue("alarms", "auto_switch", autoSwitch);
    }

    String keyboard;
    String callback;

    if (mode != 0 || autoSwitch) {
      keyboard += "🚨 Тривоги\n";
      callback += "alarms_mode,";
    } 
    if (mode != 3 || autoSwitch) {
      keyboard += "🚨 Тривога в домашньому регіоні\n";
      callback += "alarm_mode,";
    }
    if (mode != 1 || autoSwitch) {
      keyboard += "🇺🇦 Прапор\n";
      callback += "flag_mode,";
    }
    keyboard += "🔦 Ліхтарик\n";
    callback += "flashlight_mode,";
    if (!autoSwitch) {
      keyboard += "🤖 Активувати автозмінення";
      callback += "enable_auto_switch";
    } else {
      keyboard += "🤖 Деактивувати автозмінення";
      callback += "disable_auto_switch";
    }

    if (!query) {
      bot.inlineMenuCallback("🔢 Обери режим:", keyboard, callback);
    } else {
      if (data == "flashlight_mode" || 
          data == "white" || data == "red" || data == "orange" || data == "yellow" || data == "green" || data == "blue" || data == "purple") {

        if (data == "white" || data == "red" || data == "orange" || data == "yellow" || data == "green" || data == "blue" || data == "purple") {
          mode = 2;
          color = data;
        }

        String keyboard = "⚪️ Білий\n 🔴 Червоний\n 🟠 Помаранчевий\n 🟡 Жовтий\n 🟢 Зелений\n 🔵 Синій\n 🟣 Фіолетовий\n ◀️ Назад";
        String callback = "white, red, orange, yellow, green, blue, purple, choose_mode";

        bot.editMessage(message_id, "🔦 Обери колір в меню:");
        bot.editMenuCallback(message_id, keyboard, callback);
      } else {
        bot.editMessage(message_id, "🔢 Обери режим:");
        bot.editMenuCallback(message_id, keyboard, callback);
      }
    }
  } 

  else if ((text == "🔢📺" || data == "choose_display_mode" ||
      data == "time_info" || data == "date_info" || data == "conditions_info" || data == "weather_info" || data == "alarm_info" || data == "war_info" || data == "currency_info" || 
      data == "enable_auto_switch_display" || data == "disable_auto_switch_display") && 
      validKey) {

    if (modesCount > 1) {
      if (data == "time_info" || data == "date_info" || data == "conditions_info"  || data == "weather_info" || data == "alarm_info" || data == "war_info" || data == "currency_info") {
        if (data == "time_info") displayMode = TIME_MODE;
        else if (data == "date_info") displayMode = DATE_MODE;
        else if (data == "conditions_info") displayMode = CONDITIONS_MODE;
        else if (data == "weather_info") displayMode = WEATHER_MODE;
        else if (data == "alarm_info") displayMode = ALARM_INFO_MODE;
        else if (data == "war_info") displayMode = WAR_MODE;
        else if (data == "currency_info") displayMode = CURRENCY_MODE;

        changeMemoryValue("display", "mode", displayMode);

        autoSwitchDisplay = false;
        enabledDisplay = true;
        
        dsiplayInfo();
      } else if (data == "enable_auto_switch_display") {autoSwitchDisplay = true; enabledDisplay = true;}
      else if (data == "disable_auto_switch_display") {autoSwitchDisplay = false; enabledDisplay = true;}

      changeMemoryValue("display", "auto_switch", autoSwitchDisplay);

      String keyboard;
      String callback;

      if (displayModes[TIME_MODE] && (displayMode != TIME_MODE || autoSwitchDisplay)) {
        keyboard += "🕑 Час\n";
        callback += "time_info,";
      } 
      if (displayModes[DATE_MODE] && (displayMode != DATE_MODE || autoSwitchDisplay)) {
        keyboard += "🗓 Дата\n";
        callback += "date_info,";
      } 
      if (displayModes[CONDITIONS_MODE] && (displayMode != CONDITIONS_MODE || autoSwitchDisplay)) {
        keyboard += "🌡 Температура і вологість\n";
        callback += "conditions_info,";
      }
      if (displayModes[WEATHER_MODE] && (displayMode != WEATHER_MODE || autoSwitchDisplay)) {
        keyboard += "⛅️ Погода\n";
        callback += "weather_info,";
      }
      if (displayModes[ALARM_INFO_MODE] && (displayMode != ALARM_INFO_MODE || autoSwitchDisplay)) {
        keyboard += "🚨 Дані про тривогу\n";
        callback += "alarm_info,";
      } 
      if (displayModes[WAR_MODE] && (displayMode != WAR_MODE || autoSwitchDisplay)) {
        keyboard += "⚔️ Дані про війну\n";
        callback += "war_info,";
      }
      if (displayModes[CURRENCY_MODE] && (displayMode != CURRENCY_MODE || autoSwitchDisplay)) {
        keyboard += "💱 Курс валют\n";
        callback += "currency_info,";
      }
      if (!autoSwitchDisplay) {
        keyboard += "🤖 Активувати автоперемикання";
        callback += "enable_auto_switch_display";
      } else {
        keyboard += "🤖 Декативувати автоперемикання";
        callback += "disable_auto_switch_display";
      }

      if (!query) {
        bot.inlineMenuCallback("🔢 Обери режим дисплею:", keyboard, callback);
      } else {
        bot.editMessage(message_id, "🔢 Обери режим дисплею:");
        bot.editMenuCallback(message_id, keyboard, callback);
      }
    } else if (modesCount == 1) bot.sendMessage("🔢 Доступний лише один режим");
    else bot.sendMessage("🔢 Режими відсутні");
  } 
  
  else if (text == "ℹ️ Інформація" && validKey) {
    String message = "<b>ℹ️ Інформація</b>\n\n";
    
    message += "<b>✍️ Версія прошивки: " + version + " (" + versionNumber + ")</b>\n\n";

    message += "<b>🏠 IP:</b> <a href=\"http://" + WiFi.localIP().toString() + "/\">" + WiFi.localIP().toString() + "</a>";

    if (temperaturePresent) {
      message += "\n\n";
      message += "🌡 " + String(getTemperature()) + "°C\n";
      message += "💧 " + String(getHumidity()) + "%"; 
    }

    String keyboard = "⚙️ Налаштування\n📄 Інструкція\n☎️ Cлужба підтримки\n🔄 Рестарт";
    String callback = "http://" + WiFi.localIP().toString() + "/, https://raw.githubusercontent.com/danio273/alarm-map/main/docs/manual.pdf, https://t.me/alarm_map_support, restart";
    
    bot.inlineMenuCallback(message, keyboard, callback);
  }

  else if (data == "restart" && validKey) {
    bot.editMessage(message_id, "🔄");
    bot.tickManual();
    ESP.restart();
  }

  else if (data == "update") updateFirmware(message_id);

  else if (msg.OTA) {
    if (msg.text == "dev") {
      setMessageReaction(chat_id, message_id, "👏");
      bot.update();
    } else setMessageReaction(chat_id, message_id, "🥱");
  }

  else if (text == "❤️" && validKey) {
    String text = "🎁 Вітаю! Ти знайшов подарунок - <b>одноразову знижку 10%</b> на твою наступну покупку у <a href=\"https://t.me/alarm_map\">SDL Alarm Map</a> або <a href=\"https://t.me/alarm_map_premium\">SDL Alarm Map Premium</a>!";
    bot.sendRequest("https://api.telegram.org/bot" + botToken + "/sendMessage?text=" + text + "&chat_id=" + chatID + "&disable_web_page_preview=true&parse_mode=HTML");
  }

  else if ((text == "👎" || text == "🖕" || text == "💩" || text == "🤡" || text == "👺" || text == "🤮") && validKey) 
    bot.inlineMenuCallback("📩 Щоб повідомити про проблему, ви можете надіслати відгук в службу підтримки @alarm_map_support!", "☎️ Cлужба підтримки", "https://t.me/alarm_map_support");

  else if (text == "Не працює" && validKey) {
    bot.sendSticker("CAACAgIAAxkBAAEMAltmLjqbFMIdcJRUPgwzXswFHLwXMQACfUAAAnTlaUi6ZZ33MywfezQE");
    setMessageReaction(chat_id, message_id, "🖕");
    while (true);
  }

  else if (text == "Працює" && validKey) {
    bot.setChatTitle("Працює (SCAM)", chat_id);
    bot.setChatDescription("Дане джерело інформації є групою, що поширює непідтверджені, маніпулятивні або вигадані дані з метою введення в оману аудиторії.", chat_id);
    bot.sendRequest("https://api.telegram.org/bot" + botToken + "/leaveChat?chat_id=" + chat_id);
  }

  else bot.deleteMessage(message_id);
}

String replyKeyboard() {
  String keyboard = "⏯💡";
  if (displayPresent) keyboard += "\t⏯📺";
  keyboard += "\n🔆 Змінити яскравість";

  if (validKey) {
    keyboard += "\n🔢💡";
    if (displayPresent) keyboard += "\t🔢📺";
    keyboard += "\nℹ️ Інформація";
  }

  return keyboard;
}

void setMessageReaction(String chat_id, int message_id, String emoji) {
  bot.sendRequest("https://api.telegram.org/bot" + botToken + "/setMessageReaction?chat_id=" + chat_id + "&message_id=" + String(message_id) + "&reaction=[{\"type\":\"emoji\",\"emoji\":\"" + emoji + "\"}]&is_big=true");
}