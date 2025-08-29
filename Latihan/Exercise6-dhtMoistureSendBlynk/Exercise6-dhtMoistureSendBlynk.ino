#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6-oQtxAn8"
#define BLYNK_TEMPLATE_NAME "Latihan Dht"
#define BLYNK_AUTH_TOKEN "E35DXG3peFjV3Q80oZhrnClhFGqiC81i"

#define DHTPIN 5
#define DHTTYPE DHT11

const int soilMoisturePin = 36;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include "DHT.h"

char ssid[] = "Ws-Eletronika";
char pass[] = "@bpvppdg25";

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHTPIN, DHTTYPE);

unsigned int lastSendTime = 0;
const int intervalSend = 2000;

void setup() {

  Serial.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  dht.begin();
  lcd.init();
  lcd.backlight();
}

void loop() {


  float t = dht.readTemperature();
  float h = dht.readHumidity();

  int nilaiAdc = analogRead(soilMoisturePin);
  int soilMoisturePercent = map(nilaiAdc, 2870, 1090, 0, 100);
  soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);

  if (millis() - lastSendTime >= intervalSend) {

    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, h);
    Blynk.virtualWrite(V2, soilMoisturePercent);

    lastSendTime = millis();
  }



  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C \n"));

  Serial.print(soilMoisturePercent);

  lcd.setCursor(0, 0);
  lcd.print("H: ");
  lcd.print(h);
  lcd.print("%  T: ");
  lcd.print(t);
  lcd.print("°C");
  lcd.setCursor(0, 1);
  lcd.print("Kelembapan: ");
  lcd.print(soilMoisturePercent);
  lcd.print("%");
}