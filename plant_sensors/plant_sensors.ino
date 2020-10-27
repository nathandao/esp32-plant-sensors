#include <WiFi.h>
#include <PubSubClient.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>
#include "config.h"

#if PRINT_DEBUG_INFO
  #define Sprint(a) (Serial.print(a))
  #define Sprintln(a) (Serial.println(a))
#else
  #define Sprint(a)
  #define Sprintln(a)
#endif

unsigned long publishInterval = PUBLISH_INTERVAL_SECONDS * 1000UL;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup() {
  if (PRINT_DEBUG_INFO) {
    Serial.begin(BAUD_RATE);
    delay(10);
  }

  // Connect to WiFi network
  Sprintln();
  Sprint("Connecting to ");
  Sprintln(WIFI_SSID);

  WiFi.setHostname(WIFI_HOSTNAME);
  delay(10);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  delay(10);

  int tries = 0;
  // Attempt connection for 30 seconds
  while (WiFi.status() != WL_CONNECTED && tries < 120) {
    tries++;
    Sprint(".");
    delay(500);
  }

  // If still failed to connect to Wifi, restart the board
  if (WiFi.status() !=  WL_CONNECTED) {
    Sprintln("Failed to connect to Wifi. Restarting...");
    forcedRestart();
    delay(1000);
  }

  Sprint("WiFi connected, IP: ");
  Sprintln(WiFi.localIP());

  client.setServer(MQTT_SERVER, MQTT_PORT);
  checkMqttConnection();
  readSensors();
  publishReadings();
}

void loop() {
  if ( WiFi.status() !=  WL_CONNECTED ) {
    Sprintln("Connection to Wifi lost. Restarting...");
    forcedRestart();
    delay(1000);
  }

  checkMqttConnection();
  readSensors();
  publishReadings();

  delay(publishInterval);
}

void forcedRestart() {
  esp_task_wdt_init(1,true);
  esp_task_wdt_add(NULL);
  while(true);
}

void readSensors() {
  // Read all sensors' data
  for (int i = 0; i < PLANT_COUNT; i++) {
    Plant plant = plants[i];
    int reading = 0;

    for(int t = 0; t < AVERAGE_SIZE; t++) {
      reading = reading + analogRead(plant.sensorPin);
    }

    plants[i].averageReading = reading / AVERAGE_SIZE;

    int percent = getPlantMoisturePercentage(plant);
    Sprint(plant.name);
    Sprint(": ");
    Sprint(plant.averageReading);
    Sprint(", ");
    Sprint(percent);
    Sprintln("%");
  }
}

int getPlantMoisturePercentage(Plant plant) {
  int percent = map(plant.averageReading, plant.dryReading, plant.wetReading, 0, 100);
  return percent;
}

void checkMqttConnection() {
  int tries = 0;
  while (!client.connected()) {
    tries++;
    Sprintln("Attempting MQTT connection...");
    String clientId = (String)WIFI_HOSTNAME + "_mqtt_client";
    if(client.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Sprintln("Connected to MQTT server");
    } else {
      Sprintln("Failed to connect to MQTT. Retrying...");
      delay(5000);
    }
    if (tries == 10) {
      forcedRestart();
      delay(1000);
    }
  }
}

void publishReadings() {
  for (int i = 0; i < PLANT_COUNT; i++) {
    Plant plant = plants[i];
    int percent = getPlantMoisturePercentage(plant);

    String topic = MQTT_TOPIC_PREFIX + plant.name + MQTT_TOPIC_POSTFIX;
    char payload[5];
    itoa(percent, payload, 10);
    client.publish(topic.c_str(), payload);

    Sprint("Published to ");
    Sprint(topic);
    Sprint(" with payload ");
    Sprintln(payload);

    if (PUBLISH_RAW_DATA) {
      String topicRaw = topic + "-raw";
      char rawPayload[5];
      itoa(plant.averageReading, rawPayload, 10);
      client.publish(topicRaw.c_str(), rawPayload);

      Sprint("Published to ");
      Sprint(topicRaw);
      Sprint(" with payload ");
      Sprintln(rawPayload);
    }
  }
}
