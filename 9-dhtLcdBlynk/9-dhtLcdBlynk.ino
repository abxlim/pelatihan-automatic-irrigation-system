#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL62SWPsCwZ"
#define BLYNK_TEMPLATE_NAME "Sensor Monitoring"
#define BLYNK_AUTH_TOKEN "R_37n65FWiU03xpg_gbLxDFF2_JUI28s"

#define DHTPIN 4
#define DHTTYPE DHT11

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

char ssid[] = "Ws-Eletronika";
char pass[] = "@bpvppdg25";

LiquidCrystal_I2C lcd(0x27, 20, 4);  // 20 karakter, 4 baris

const int btBack = 13;
const int btUp = 12;
const int btDown = 14;
const int btOk = 27;

DHT dht(DHTPIN, DHTTYPE);

const int totalMenu = 2;  // menu 0,1,2 → total = 3 pilihan
bool setMenuState = false;
int menu = 0;
int nilaiSet = 700;

void setup() {

  Serial.begin(9600);

  // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(btBack, INPUT_PULLUP);
  pinMode(btUp, INPUT_PULLUP);
  pinMode(btDown, INPUT_PULLUP);
  pinMode(btOk, INPUT_PULLUP);

  dht.begin();

  lcd.init();
  lcd.backlight();

  tampilMenu();
}

void loop() {
  setNilai();

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C \n"));

  // lcd.setCursor(0, 0);
  // lcd.print("Hum : ");
  // lcd.print(h, 1);
  // lcd.print(" %");
  // lcd.setCursor(0, 1);
  // lcd.print("Temp: ");
  // lcd.print(t, 1);
  // lcd.print(char(223));
  // lcd.print("C");

  delay(2000);

  if (!setMenuState) {
    if (digitalRead(btUp) == LOW) {
      menu++;
      if (menu > totalMenu) menu = 0;
      tampilMenu();
      delay(200);
    }

    else if (digitalRead(btDown) == LOW) {
      menu--;
      if (menu < 0) menu = totalMenu;
      tampilMenu();
      delay(200);
    }
  }

  if (digitalRead(btOk) == LOW) {
    delay(200);
    setMenuState = true;

    if (setMenuState == true && menu != 0) {
      if (menu == 1) tampilSet("SETTING", nilaiSet);
      if (menu == 2) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("...");
      }
      delay(200);
    }
  }

  else if (digitalRead(btBack) == LOW) {
    delay(200);
    tampilMenu();
    setMenuState = false;
  }

  if (setMenuState && menu == 0) {
    tampilSet("SUHU", t);
    delay(200);
  }
}


void tampilMenu() {
  lcd.clear();
  if (menu == 0) lcd.print("> SUHU");
  else if (menu == 1) lcd.print("> SETTING");
  else if (menu == 2) lcd.print("> RUN PROGRAM");
}

void tampilSet(String label, int nilai) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label);
  lcd.setCursor(0, 1);
  lcd.print("Nilai : ");
  lcd.print(nilai);
}

void setNilai() {
  if (setMenuState && menu == 1) {
    if (digitalRead(btUp) == LOW) {
      delay(200);
      if (setMenuState == true && menu == 1) {
        nilaiSet++;
      }
      tampilSet("SETTING", nilaiSet);
    }

    else if (digitalRead(btDown) == LOW) {
      delay(200);
      if (setMenuState == true && menu == 1) {
        nilaiSet--;
      }
      tampilSet("SETTING", nilaiSet);
    }
  }
}