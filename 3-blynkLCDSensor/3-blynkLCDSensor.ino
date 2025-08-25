

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

char ssid[] = "Ws-Eletronika";
char pass[] = "@bpvppdg25";

const int soilPin = 36;
const int LEDPin = 2;

int lastPersen = 0;
unsigned long lastTime = 0;
const int interval = 200;

void setup() {
  // Debug console
  Serial.begin(9600);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  lcd.init();
  lcd.backlight();
  pinMode(LEDPin, OUTPUT);  // Atur pin D2 sebagai OUTPUT
}

void loop() {

  unsigned long currentTime = millis();

  int nilai = analogRead(soilPin);
  nilai = constrain(nilai, 990, 2890);
  int persen = map(nilai, 2890, 990, 0, 100);

  if (persen > 50) {
    digitalWrite(LEDPin, HIGH);
    Blynk.virtualWrite(V1, HIGH);
  } else {
    digitalWrite(LEDPin, LOW);
    Blynk.virtualWrite(V1, LOW);
  }

  if (currentTime - lastTime >= interval) {

    lastTime = millis();
    if (lastPersen != persen) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Nilai ADC: ");
      lcd.setCursor(12, 0);
      lcd.print(nilai);
      lcd.setCursor(0, 1);
      lcd.print(persen);
      lcd.setCursor(3, 1);
      lcd.print("%");

      Blynk.virtualWrite(V0, persen);

      lastPersen = persen;
    }
  }
}