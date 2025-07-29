#define ALARMS_ID 0
#define AUTO_BRIGHTNESS_ID 1
#define STRIP_ID 2
#define DISPLAY_ID 3
#define DIPSLAY_TIME_ID 4
#define TEMPERATURE_SINRIC_ID 5

void initOS() {
  #ifdef DEBUG
    Serial.println("Ініціалізація OS...");
  #endif

  OS.attach(ALARMS_ID, checkAlarms, alarmsInterval);
  checkAlarms();

  if (autoBrightnessSource == 0) {
    OS.attach(AUTO_BRIGHTNESS_ID, [](){if (autoBrightness) timeAutoBrightness();}, 60000);
    if (autoBrightness) timeAutoBrightness();
  } else if (autoBrightnessSource == 1) {
    OS.attach(AUTO_BRIGHTNESS_ID, [](){if (autoBrightness) sensorAutoBrightness();}, 5000);
    if (autoBrightness) sensorAutoBrightness();
  }

  OS.attach(STRIP_ID, displayStrip, 2000);
  displayStrip();

  if (displayPresent && modesCount > 0) {
    OS.attach(DISPLAY_ID, [](){if (enabledDisplay) checkAutoSwitchDisplay();}, autoSwitchDisplayInterval);
    OS.attach(DIPSLAY_TIME_ID, [](){if (enabledDisplay && displayMode == TIME_MODE) displayTime();}, 1000);
    if (enabledDisplay) checkAutoSwitchDisplay();
    if (enabledDisplay && displayMode == TIME_MODE) displayTime();
  }

  // if (temperaturePresent && !sinricTemperatureId.isEmpty()) {
  //   OS.attach(TEMPERATURE_SINRIC_ID, handleTemperaturesensor, 600000);
  //   handleTemperaturesensor();
  // }
}