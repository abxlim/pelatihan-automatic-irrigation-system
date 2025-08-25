#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);  // 20 karakter, 4 baris

const int btBack = 13;
const int btUp = 12;
const int btDown = 14;
const int btOk = 27;

const int sensorPin = 36;

const int totalMenu = 2;  // menu 0,1,2 → total = 3 pilihan

bool setMenuState = false;

int menu = 0;

int nilaiSet = 700;

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(btBack, INPUT_PULLUP);
  pinMode(btUp, INPUT_PULLUP);
  pinMode(btDown, INPUT_PULLUP);
  pinMode(btOk, INPUT_PULLUP);

  tampilMenu();
}

void loop() {

  setNilai();

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
    tampilSet("SUHU", analogRead(sensorPin));
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
