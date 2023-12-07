#include <Wire.h>
#include <DHT.h>
#include "variables.h"

// setup
void setup() {
  Wire.begin();
  Serial.begin(SERIAL_PORT);
  WiFi.mode(WIFI_STA);

  // while (WiFi.status() != WL_CONNECTED)
    wifiConnect();
  
  initMqtt(callback);
  
  // Begin DHT sensor reading
  dht.begin();
}

// main
void loop() {
  if (millis() > timer) {
    Serial.println("sending telemetry...");

    String payload = "{\"rssi\": "+ String(WiFi.RSSI()) +", \"Humidity\": "+ String(dht.readHumidity()) +", \"Temperature\": "+ String(dht.readTemperature()) + "}";
    client.publish(PUBLISH_TOPIC, payload.c_str());

    // sendTelemetry("rssi", String(WiFi.RSSI()));
    // sendTelemetry("Humidity", String(dht.readHumidity()));
    // sendTelemetry("Temperature", String(dht.readTemperature()));
    timer = millis() + (120 * 1000);
  }

  if (WiFi.status() != WL_CONNECTED)
    reconnWifi();
  
  client.loop();
}

void callback(char *topic, byte *payload, uint lenght) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("message :");
  for (int i=0; i<lenght; i++)
    Serial.print((char) payload[i]);
  
  Serial.println();
  Serial.println("--------------------------------");
}

void sendTelemetry(String key, String value) {
  String payload = "";
  payload = "{\"" + key + "\": " + value + "}";

  client.publish(PUBLISH_TOPIC, payload.c_str());
}

// initialize wifi connection (use in setup state)
void wifiConnect() {
  char *ssid = SSID;
  char *password = PASSWORD;

  if (fallback) {
    ssid = FALLBACK_SSID;
    password = FALLBACK_PASSWORD;
  }

  WiFi.begin(ssid, password);
  
  Serial.println("Connecting Wifi...");

  while (WiFi.status() != WL_CONNECTED) {
    unsigned long currMillis = millis();

    delay(500);
    Serial.print(".");

    if (currMillis - pervMillis <= wifi_timeout) {
      WiFi.disconnect();
      fallback = !fallback;
      break;
    }

    pervMillis = currMillis;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("#");
    Serial.println("Wifi Connection Success!");
    Serial.print("Local IP is: ");
    Serial.println(WiFi.localIP());
  }
}

// reconnect to wifi connection (use in loop state)
void reconnWifi() {
  Serial.println("Reconnecting Wifi...");
  WiFi.disconnect();
  wifiConnect();
}

// connect to mqtt server (thingsboard)
void initMqtt(void (*callback)(char *, uint8_t *, uint)) {
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.printf("The client %s connects to the MQTT broker\n", MQTT_DEVICE);
    if (client.connect(MQTT_DEVICE, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("MQTT broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  client.subscribe(SUBSCRIBE_TOPIC);
}