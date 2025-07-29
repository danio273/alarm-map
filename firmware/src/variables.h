// #define DEBUG 1 

String version;
int versionNumber;
String updateInfo[4];

String accesKey;
bool validKey;
String checkKeyMessage;

String ssid, password;
long wifiConnectTime; 

String botToken, chatID;

int alarmsSource;
String alarmsToken;
long alarmsInterval;
bool greenStates;
int statesOrder;
int kyivMode;
String homeCity;

int soundSource;
int buzzerPin;
long buzzerInterval;
int speakerVolume;

bool integrationPresent;
int integrationPin;
bool integrationHigh;

int stripPin;
String stripType;
bool enabledStrip;
int brightness;
bool autoSwitch;
bool silenceMinuteEnabled;

int autoBrightnessSource;
bool autoBrightness;

int dayHour, dayMinute, nightHour, nightMinute;
int dayBrightness, nightBrightness;

int photoresistorPin;
int lightMax, lightMin;

bool displayPresent;
String displayCommunication;
int displayWidth, displayHeight;
bool displayFlip; 
bool displayModes[7]; 
long autoSwitchDisplayInterval;
bool enabledDisplay;
bool autoSwitchDisplay; 

String weatherToken;
int homeCityId;

bool temperaturePresent;
int temperaturePin;
int temperatureType;
float temperatureOffset;

String sinricStripId, sinricTemperatureId;
String sinricKey;
String sinricSecret;

String ntpHost;
int timeZone; 