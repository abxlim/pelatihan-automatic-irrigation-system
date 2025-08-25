#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL62SWPsCwZ"
#define BLYNK_TEMPLATE_NAME "Sensor Monitoring"
#define BLYNK_AUTH_TOKEN "R_37n65FWiU03xpg_gbLxDFF2_JUI28s"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi credentials
char ssid[] = "Ws-Eletronika";
char pass[] = "@bpvppdg25";

// LCD object: address 0x27, 20 characters x 4 lines
LiquidCrystal_I2C lcd(0x27, 20, 4);

const int soilResistPin = 33;
unsigned int currentTime = 0;
const int intervalSend = 2000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
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

  // Print data to Serial monitor
  lcd.setCursor(0, 0);
  lcd.print("Kelembapan: ");
  lcd.print(soilPercent);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(F("Nilai ADC: "));
  lcd.print(nilaiADC);

  delay(100);
}
