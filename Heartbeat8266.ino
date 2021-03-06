/*
  Basic ESP8266 MQTT example
  This sketch demonstrates the capabilities of the pubsub library in combination
  with the ESP8266 board/library.
  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
  It will reconnect to the server if the connection is lost using a blocking
  reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
  achieve the same result without blocking the main loop.
  To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "credentials.h"

// If you do not want to use a credentials.h file, comment it and uncomment the lines below:
/* #define mySSID "xxxx" // wifi name
  #define myPASSWORD "yyyyyyyyyyyyyy" // wifi password
  #define TELEGRAM_CERTIFICATE_ROOT "cert" // telegram cert
  #define CHAT_ID "zzzzzzzzz" // telegram chat id
  #define ALARM_BOT_TOKEN "aaaaa" // telegram bot
*/

//const char* ssid = mySSID;
//const char* password = myPASSWORD;
//const char* mqtt_server = "192.168.0.203";

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(ALARM_BOT_TOKEN, secured_client);

unsigned long lastMsg = millis();
int failedConnections = 0;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long interval;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
inline int32_t timeDiff(const unsigned long prev, const unsigned long next) {
  return ((int32_t)(next - prev));
}

inline long timePassedSince(unsigned long timestamp) {
  return timeDiff(timestamp, millis());
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup_wifi() {
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  interval = millis();

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_TOPIC_IN);
      failedConnections = 0;
      Serial.println(" ");
      Serial.print("failedConnections: ");
      Serial.println(failedConnections);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      Serial.print("Alarm ");
      Serial.println(failedConnections);
      // Wait 5 seconds before retrying
      for (byte i = 0; i < 10; i++) delay(500); // delay(5000) may cause hang
      failedConnections++;
      Serial.println(" ");
      Serial.print("failedConnections: ");
      Serial.println(failedConnections);
      if (failedConnections >= 10) {

        bot.sendMessage(CHAT_ID, "MQTT does not work", "");
        failedConnections = 0;
      }
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Setup
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
  Serial.begin(57600);
  setup_wifi();
  Serial.print("Retrieving time: ");
  configTime(0, 0, "asia.pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
  bot.sendMessage(CHAT_ID, "HeartBeat Bot started up", "");
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback);
  interval = millis();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//loop
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (timeDiff(lastMsg, millis()) > 60000) {
    lastMsg = millis();
    Serial.print("Publish heartbeat: ");
    Serial.print(millis());
    Serial.println(" msec");
    client.publish(MQTT_TOPIC_OUT, "heartbeatESP8266");
  }

  if (timeDiff(interval, millis()) > 130000) {
    Serial.println("Alarm, Hub Heartbeat does not work");
    bot.sendMessage(CHAT_ID, "Hub Heartbeat does not work", "");
    interval = millis();
  }
}
