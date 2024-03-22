// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#ifndef STASSID
#define STASSID "WIFI_MODS"
#define STAPSK "CACADEBACA"
#endif
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Reemplaza con tus credenciales de red WiFi
const char* ssid = "WIFI_MODS";
const char* password = "CACADEBACA";

// Dirección IP del broker MQTT
const char* mqtt_server = "10.1.105.26";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

#define DHTPIN 5          // Pin D1 en ESP8266
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

unsigned long previousMillis = 0;  // Almacenará la última vez que se actualizó el DHT
const long interval = 10000;       // Intervalo para actualizaciones DHT (10 segundos)

void setup_wifi() {
  delay(10);
  // Conectarse a la red WiFi
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Bucle hasta que estemos reconectados
  while (!mqttClient.connected()) {
    Serial.print("Intentando conexión MQTT...");
    if (mqttClient.connect("ESP8266Client")) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" intenta de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  mqttClient.setServer(mqtt_server, 1883);
  dht.begin();
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Lee la temperatura como Celsius (el valor predeterminado)
    float newT = dht.readTemperature();
    // Lee la humedad
    float newH = dht.readHumidity();

    if (!isnan(newT) && !isnan(newH)) {
      String createdBy = "Hector";
      String message = String(newT) + "-" + String(newH) + "-" + String(createdBy);
      // Publica los valores de temperatura y humedad
      mqttClient.publish("ff/temp", message.c_str());
      //mqttClient.publish("ff/hum", String(newH).c_str());
      /*
      Serial.print("Temperatura: ");
      Serial.print(newT);
      Serial.print("C, Humedad: ");
      Serial.print(newH);
      Serial.println("%");
      */
    } else {
      Serial.println("Error al leer del sensor DHT");
    }
  }
}
