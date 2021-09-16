#ifndef CREDENTIALS_h
#define CREDENTIALS_h

// local AP
const char* ssid     = " ";
const char* password = " ";

// static IP
const IPAddress ip(192, 168, 2, 122);
// const IPAddress dns(8, 8, 8, 8);
const IPAddress gateway(192, 168, 2, 1);
const IPAddress subnet(255, 255, 255, 0);

// MQTT Server
const char* MQTT_SERVER    = "raspberrypi.local";
const uint16_t MQTT_PORT   = 1883;
const char* MQTT_ID        = "ESP8266-WD";
const char* MQTT_USER      = "admin";
const char* MQTT_PSWD      = "admin";

// MQTT Topic
const char* MQTT_TOPIC_OUT = "heartbeatHUB/#";
const char* MQTT_TOPIC_IN  = "heartbeatHUB";

//TELEGRAM
//#define TELEGRAM_CERTIFICATE_ROOT "cert" // telegram cert
#define CHAT_ID " " // telegram chat id
#define ALARM_BOT_TOKEN " " // telegram bot

const char *NTP_SERVER = "asia.pool.ntp.org"; //"pool.ntp.org"

#endif
