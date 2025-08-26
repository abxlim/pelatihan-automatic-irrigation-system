#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

RTC_DS3231 rtc;
DateTime now;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int ledPin = 13;
bool ledState = false;
bool stateAlarm = false;
bool displayAlarmLCD = false;
bool alarmActive = false;
unsigned int lastPrint = 0;
unsigned int lastBlink = 0;
unsigned int lastDisplayTime = 0;
int intervalPrint = 1000;
int intervalBlink = 200;
int intervalDisplay = 1000;

const int setAlarmHour = 14;
const int setAlarmMinute = 19;

char daysOfTheWeek[7][12] = { "Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu" };

void setup() {
  Serial.begin(57600);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  lcd.init();
  lcd.backlight();

  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(ledPin, OUTPUT);
}

void loop() {
  now = rtc.now();

  if (millis() - lastPrint >= intervalPrint) {
    tampilLCD();
    lastPrint = millis();
  }


  if ((now.hour() == setAlarmHour) && (now.minute() >= setAlarmMinute) && (now.minute() < setAlarmMinute + 2)) {

    displayAlarmLCD = false;
    alarmActive = true;

    if (millis() - lastBlink >= intervalBlink) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);

      lastBlink = millis();
    }
  } else {
    if (alarmActive && !displayAlarmLCD) {
      lcd.clear();
      lcd.print("ALARM MATI");
      digitalWrite(ledPin, LOW);
      alarmActive = false;
      displayAlarmLCD = true;
      lastDisplayTime = millis();
    }
    if (displayAlarmLCD && millis() - lastDisplayTime >= intervalDisplay) {
      lcd.clear();
      displayAlarmLCD = false;
    }
  }
}

void tampilLCD() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.year());
  lcd.setCursor(0, 1);
  lcd.print(now.hour());
  lcd.print(":");
  lcd.print(now.minute());
  lcd.print(":");
  lcd.print(now.second());
  lcd.setCursor(10, 1);
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
}
