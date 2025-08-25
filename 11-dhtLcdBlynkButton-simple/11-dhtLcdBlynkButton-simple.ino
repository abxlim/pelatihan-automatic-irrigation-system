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

// WiFi credentials
char ssid[] = "Ws-Eletronika";
char pass[] = "@bpvppdg25";

// LCD object: address 0x27, 20 characters x 4 lines
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Button pins
const int btBack = 13;
const int btUp = 12;
const int btDown = 14;
const int btOk = 27;

// DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

// Menu variables
const int totalMenu = 2;  // menu options: 0,1,2 → total 3 options
bool setMenuState = false;
int menu = 0;
int nilaiSet = 700;
unsigned int currentTime = 0;
const int intervalSend = 2000;



void setup() {
  Serial.begin(9600);

  // Uncomment this to enable Blynk connection
  // Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Setup buttons as input with pullup resistors
  pinMode(btBack, INPUT_PULLUP);
  pinMode(btUp, INPUT_PULLUP);
  pinMode(btDown, INPUT_PULLUP);
  pinMode(btOk, INPUT_PULLUP);

  // Initialize DHT sensor
  dht.begin();

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  tampilMenu();  // Show initial menu
}

void loop() {
  setNilai();  // Handle setting changes if in setting mode

  // Read sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (millis() - currentTime >= intervalSend) {
    // Send data to Blynk app
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);

    Serial.print("Test");

    currentTime = millis();
  }



  // Print data to Serial monitor
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C"));

  // Menu navigation when not in setting mode
  if (!setMenuState) {
    if (digitalRead(btUp) == LOW) {
      menu++;
      if (menu > totalMenu) menu = 0;
      tampilMenu();
      delay(200);
    } else if (digitalRead(btDown) == LOW) {
      menu--;
      if (menu < 0) menu = totalMenu;
      tampilMenu();
      delay(200);
    }
  }

  // Ok button pressed: enter setting mode or show submenus
  if (digitalRead(btOk) == LOW) {
    delay(200);
    setMenuState = true;

    if (menu != 0) {
      if (menu == 1) {
        tampilSet("SETTING", nilaiSet);
      } else if (menu == 2) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("...");
      }
      delay(200);
    }
  }

  // Back button pressed: return to main menu and exit setting mode
  if (digitalRead(btBack) == LOW) {
    delay(200);
    tampilMenu();
    setMenuState = false;
  }

  // If in setting mode and menu 0 (temperature), display temperature
  if (setMenuState && menu == 0) {
    tampilSet("SUHU", temperature);
    delay(200);
  }
}

// Display main menu options
void tampilMenu() {
  lcd.clear();
  switch (menu) {
    case 0:
      lcd.print("> SUHU");
      break;
    case 1:
      lcd.print("> SETTING");
      break;
    case 2:
      lcd.print("> RUN PROGRAM");
      break;
  }
}

// Display setting or sensor value on LCD
void tampilSet(String label, float nilai) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label);
  lcd.setCursor(0, 1);
  lcd.print("Nilai : ");

  // Print nilai with appropriate unit and formatting
  if (menu == 0) {  // SUHU (temperature)
    lcd.print(nilai, 1);
    lcd.print(char(223));  // Degree symbol
    lcd.print("C");
  } else if (menu == 1) {  // SETTING
    lcd.print((int)nilai);
    lcd.print("%");
  } else {
    lcd.print(nilai);
  }
}

// Handle increment/decrement of nilaiSet in setting mode
void setNilai() {
  if (setMenuState && menu == 1) {  // Only for SETTING menu
    if (digitalRead(btUp) == LOW) {
      delay(200);
      nilaiSet++;
      tampilSet("SETTING", nilaiSet);
    } else if (digitalRead(btDown) == LOW) {
      delay(200);
      nilaiSet--;
      tampilSet("SETTING", nilaiSet);
    }
  }
}
