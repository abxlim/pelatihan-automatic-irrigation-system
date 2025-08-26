#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL62SWPsCwZ"
#define BLYNK_TEMPLATE_NAME "Sensor Monitoring"
#define BLYNK_AUTH_TOKEN "R_37n65FWiU03xpg_gbLxDFF2_JUI28s"

#define DHTPIN 32
#define DHTTYPE DHT11

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

// WiFi credentials
char ssid[] = "Ws-Eletronika";
char pass[] = "@bpvppdg25";

const int intervalSend = 1000;
int currentTime = 0;

// DHT sensor object
DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(9600);

  // Uncomment this to enable Blynk connection
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Initialize DHT sensor
  dht.begin();
}

void loop() {

  // Read sensor data
  float temperature = dht.readTemperature();

  if (millis() - currentTime >= intervalSend) {
    // Send data to Blynk app

    if (temperature > 38) {
      Blynk.virtualWrite(V4, 1);
      Blynk.virtualWrite(V5, "Tolong!!!");
    } else {
      Blynk.virtualWrite(V4, 0);
      Blynk.virtualWrite(V5, "         ");
    }

    currentTime = millis();
  }
  Serial.println(temperature);
}
