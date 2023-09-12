#include <WiFi.h>
#include <PubSubClient.h>
//MQTT
#define WIFI_SSID "Lugo"
#define WIFI_PASSWORD "olimpia1902"

#define MQTT_SERVER "192.168.0.33"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_TOPIC "alarma/esp/node1"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//----------
//DHT11
#include "DHT.h"
#define DHTPIN 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int MQ2_PIN = 34;
int led = 4;
int estado;
void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Conectando al WiFi...");
    delay(1000);
  }
  Serial.println("Conectado al WiFi!");
  
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  
}

void loop() {
  if (!mqttClient.connected()) {
    connectToMqtt();
  }
  mqttClient.loop();
  int sensor2 = analogRead(MQ2_PIN);
//------------
//DHT11
float hum = dht.readHumidity();
float temp = dht.readTemperature();
if (sensor2 >= 3300 && temp >= 25) {
  digitalWrite(led, HIGH);
  estado=1;
}else {
  digitalWrite(led, LOW);
  estado=0;
}
char payload[200];
  sprintf(payload, "{ \"mq2\": %i, \"temp\": %.2f, \"hum\": %.2f, \"estado\": %i }", sensor2, temp, hum, estado);
  mqttClient.publish(MQTT_TOPIC, payload);
  Serial.print(sensor2);
  Serial.print(" / ");
  Serial.print(temp);
  Serial.print(" / ");
  Serial.print(hum);
  Serial.print(" / ");
  Serial.println(estado);
  delay(10000);
}

void connectToMqtt() {
  while (!mqttClient.connected()) {
    Serial.println("Conectando al broker MQTT...");
    if (mqttClient.connect("ESP32", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Conectado al broker MQTT!");
      mqttClient.subscribe("comandos");
    } else {
      Serial.print("Fallo al conectar al broker MQTT: ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
