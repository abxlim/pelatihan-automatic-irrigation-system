#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL62SWPsCwZ"
#define BLYNK_TEMPLATE_NAME "Sensor Monitoring"
#define BLYNK_AUTH_TOKEN "R_37n65FWiU03xpg_gbLxDFF2_JUI28s"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// WiFi credentials
char ssid[] = "Ws-Eletronika";
char pass[] = "@bpvppdg25";

const int soilResistPin = 33;
unsigned int currentTime = 0;
const int intervalSend = 2000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  // put your main code here, to run repeatedly:
  int nilaiADC = analogRead(soilResistPin);
  int soilPercent = map(nilaiADC, 4095, 1630, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);
  Serial.print(soilPercent);
  Serial.println("%");

  if (millis() - currentTime >= intervalSend) {
    // Send data to Blynk app
    Blynk.virtualWrite(V2, soilPercent);
    Blynk.virtualWrite(V3, nilaiADC);

    Serial.print("Test");

    currentTime = millis();
  }
  delay(100);
}
