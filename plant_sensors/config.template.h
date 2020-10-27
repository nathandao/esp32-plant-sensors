// No need to modify this
typedef struct {
  String name;
  int sensorPin;
  int dryReading;
  int wetReading;
  int averageReading;
} Plant;

// Only needed when PRINT_DEBUD_INFO is true to read values from Serial.print
// Default to baud rate 115200
#define BAUD_RATE 115200

// For each reading, take the average over this amount of sample size
// instead of relying on a single sensor reading.
#define AVERAGE_SIZE 100

// Setting this to true will print debug values to the Serial monitor
#define PRINT_DEBUG_INFO false

// Wifi settings
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
// If you're using more than 1 ESP32 boards and connect them to the same
// network, remember to set a unique hostname for each
#define WIFI_HOSTNAME "plant_moisture_sensors"

// MQTT connection
#define MQTT_USER "your_mqtt_user"
#define MQTT_PASSWORD "your_mqtt_password"
#define MQTT_SERVER "your_mqtt_hostname_or_ip"
#define MQTT_PORT 1883 // or use 8883 for more secured SSL connection


// By default, the ESP32 will publish your plant data to 2 topics:
//
// username/feeds/plant.your-plant-name-moisture with the converted value in percent
//
// If PUBLISH_DRAW_DATA is set to true, the raw reading data will also
// be published to username/feeds/plant.your-plant-name-moisture-raw
// with the raw sensor readings.
#define MQTT_TOPIC_PREFIX "username/feeds/plant."
#define MQTT_TOPIC_POSTFIX "-moisture"
#define PUBLISH_RAW_DATA false

// Durations between each publish in seconds
#define PUBLISH_INTERVAL_SECONDS 60

// For some reason using sizeof(plants) will crash the ESP32.
// So setting PLANT_COUNT as a dirty hack :sweat-smile:
// Set the number of moisture sensors you're connecting to the board.
#define PLANT_COUNT 3

// The array of plants, with values set in this order
// { "plant-name", sensorPinNumber, dryReading, wetReading, averageReading }
//
// dryReading and wetReading need to be manually calibrated for each
// sensor by checking the dryReading when the sensor if lifted in the air,
// without any contact to water, and wetReading when the sensor is summerged
// in a glass of water.
Plant plants[PLANT_COUNT] = {
  { "plant-1-name", 37, 3327, 1710, 0 },
  { "plant-2-name", 38, 3354, 1740, 0},
  { "plant-3-name", 39, 3435, 1225, 0 },
};
