#define BLYNK_PRINT Serial

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6M2kgtP6J"
#define BLYNK_TEMPLATE_NAME "LED Kontrol"
#define BLYNK_AUTH_TOKEN "FDq3vkQTa6Lx-aRqIC__IIHlMLzl7lk2"


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Ws-Eletronika";
char pass[] = "@bpvppdg25";

void setup() {
  // Debug console
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  lcd.init();
  lcd.backlight();
  pinMode(2, OUTPUT);  // Atur pin D2 sebagai OUTPUT
}

void loop() {
  Blynk.run();

  Blynk.virtualWrite(V0, 1);
  digitalWrite(2, HIGH);
  delay(1000);
  Blynk.virtualWrite(V0, 0);
  digitalWrite(2, LOW);
  delay(1000);
}