String apiToken = "";
String tableName = "";
String columnId = "";

bool checkKey(String key, String& message) {
  #ifdef DEBUG
    Serial.println("Перевірка ключа " + key + ".");
  #endif

  if (key.length() == 0) {
    message = "Ключ відсутній!";
    return false;
  }

  if (key.length() != 15) {
    message = "Ключ неправильний!";
    return false;
  }

  for (int i = 0; i < 3; i++) {
    if (!isAlpha(key[i])) {
      message = "Ключ неправильний!";
      return false;
    }
  }

  for (int i = 3; i < 7; i++) {
    if (!isDigit(key[i])) {
      message = "Ключ неправильний!";
      return false;
    }
  }

  for (int i = 7; i < 10; i++) {
    if (!isAlpha(key[i])) {
      message = "Ключ неправильний!";
      return false;
    }
  }

  for (int i = 10; i < 14; i++) {
    if (!isDigit(key[i])) {    
      message = "Ключ неправильний!";
      return false;
    }
  }

  if (key[14] != '!' && key[14] != '@' && key[14] != '#' && key[14] != '$' &&
      key[14] != '%' && key[14] != '&' && key[14] != '?' && key[14] != '/') {
    message = "Ключ неправильний!";
    return false;
  }

  HTTPClient http;

  http.begin("https://cloud.seatable.io/api/v2.1/dtable/app-access-token/");
  http.addHeader("accept", "application/json");
  http.addHeader("authorization", "Bearer " + apiToken);

  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    String response = http.getString();
    http.end();

    DynamicJsonDocument doc(1000);
    deserializeJson(doc, response);
    String baseToken = doc["access_token"].as<const char*>();
    String tableUuid = doc["dtable_uuid"].as<const char*>();

    HTTPClient http;
    http.begin("https://cloud.seatable.io/api-gateway/api/v2/dtables/" + tableUuid + "/rows/?table_name=" + tableName);
    http.addHeader("accept", "application/json");
    http.addHeader("authorization", "Bearer " + baseToken);
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      String response = http.getString();
      http.end();

      DynamicJsonDocument doc(10000);
      deserializeJson(doc, response);

      String rowId;

      for (int i = 0; i < doc["rows"].size(); i++) {
        if (doc["rows"][i]["0000"] == key) {
          rowId = doc["rows"][i]["_id"].as<const char*>();

          delay(5000);

          if (doc["rows"][i]["m5qu"] == "none") {
            HTTPClient http;
            // http.begin("https://cloud.seatable.io/dtable-server/api/v1/dtables/" + columnId + "/rows/");
            http.begin("https://cloud.seatable.io/api-gateway/api/v2/dtables/" + tableUuid + "/rows/");
            http.addHeader("accept", "application/json");
            http.addHeader("authorization", "Bearer " + baseToken);
            http.addHeader("content-type", "application/json");

            // String jsonData = "{\"row\": {\"mac\": \"" + WiFi.macAddress() + "\"}, \"table_name\": \"" + tableName + "\", \"row_id\": \"" + rowId + "\"}";
            String jsonData = "{\"table_name\": \"" + tableName + "\", \"updates\": [{\"row\": {\"mac\": \"" + WiFi.macAddress() + "\"},\"row_id\": \"" + rowId + "\"}]}";


            int httpResponseCode = http.PUT(jsonData); 

            if (httpResponseCode != 200) {
              message = "Під час перевірки ключа доступу виникла помилка. Код помилки: " + String(httpResponseCode) + " (3).";
              return false;
            }

            http.end();

            message = "Ключ активовано для цього пристрою! Перевірка ключа успішна!";
            return true;
          } else if (doc["rows"][i]["m5qu"] == WiFi.macAddress()) {
            message = "Перевірка ключа успішна!";
            return true;
          } else {
            message = "Ключ був активований на іншому пристрої!";
            return false;
          }
        }
      }
      message = "Ключ доступу недійсний!";
      return false;
    } else {
      http.end();
      message = "Під час перевірки ключа доступу виникла помилка. Код помилки: " + String(httpResponseCode) + " (2).";
      return false;
    }
  } else {
    http.end();
    message = "Під час перевірки ключа доступу виникла помилка. Код помилки: " + String(httpResponseCode) + " (1).";
    return false;
  }
}