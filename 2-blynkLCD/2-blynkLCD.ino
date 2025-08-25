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

const int soilPin = 36;

void setup() {
  // Debug console
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  lcd.init();
  lcd.backlight();
  pinMode(2, OUTPUT);  // Atur pin D2 sebagai OUTPUT
}

void loop() {
  int nilai = analogRead(36);
  nilai = constrain(nilai, 990, 2890);
  int persen = map(nilai, 2890, 990, 0, 100);

  Blynk.run();
  //Blynk.virtualWrite(V0, persen);
  digitalWrite(2, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nilai ADC: ");
  lcd.setCursor(12, 0);
  lcd.print(nilai);
  lcd.setCursor(0, 1);
  lcd.print(persen);
  lcd.setCursor(3, 1);
  lcd.print("%");
  delay(2000);

  //Blynk.virtualWrite(V0, soilValue);
  digitalWrite(2, LOW);
  delay(2000);
}