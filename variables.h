#include <PubSubClient.h>
#include <WiFi.h>

// setup wifi token
constexpr char *SSID = "*** YOUR PRIMARY SSID ***";
constexpr char *PASSWORD = "*** YOUR PRIMARY PASSWORD ***";
constexpr char *FALLBACK_SSID = "*** YOUR SECONDARY SSID ***";
constexpr char *FALLBACK_PASSWORD = "*** YOUR SECONDARY PASSWORD ***";

// add check condition if wifi go to fallback state
bool fallback = false;

// set timer for abort wifi connect
unsigned long pervMillis = 0;
constexpr unsigned int wifi_timeout = 15 * 1000;

// setup mqtt credential
constexpr char *MQTT_BROKER = "*** YOUR THINGSBOARD SERVER ***";
constexpr char *SUBSCRIBE_TOPIC = "v1/devices/me/attributes";
constexpr char *PUBLISH_TOPIC = "v1/devices/me/telemetry";
constexpr char *MQTT_DEVICE = "*** YOUR DEVICE ID ***";
constexpr char *MQTT_USERNAME = "*** YOUR USERNAME ***";
constexpr char *MQTT_PASSWORD = "*** YOUR PASSWORD ***";
constexpr unsigned short MQTT_PORT = 1883;

// setup common
constexpr unsigned int SERIAL_PORT = 115200;

// setup dht22 port input
constexpr int DHT_PIN = 27;

// get device client from wifi client detail
WiFiClient espClient;
// initialize MQTT client
PubSubClient client(espClient);
// define DHT sensor pin and type
DHT dht(DHT_PIN, DHT22);
// set timer for delay data
uint timer = millis() + 2000;

void wifiConnect();
void reconnWifi();
void initMqtt(void (*callback)(char *, uint8_t *, uint));
void callback(char *topic, byte *payload, uint lenght);
void sendTelemetry(String key, String value);