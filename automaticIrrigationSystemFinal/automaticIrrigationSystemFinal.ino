/*Ini Berasal Dari Blynk Silahkan disesuaikan*/
#define BLYNK_TEMPLATE_ID "TMPL6AUPp-seV"
#define BLYNK_TEMPLATE_NAME "Monitoring Final"
#define BLYNK_AUTH_TOKEN "Z1-YOslPVa80Qb1SlbjRwBiQu57RztpG"

/*library*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <RTClib.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

/*Deklarasi Variabel LCD*/
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Alamat EEPROM (offset tiap variabel)
#define ADDR_KALIBSUHU 0     //dimulai dari alamat 0
#define ADDR_BATASSUHU 4     //dimulai dari alamat 4 agar tidak tabrakan dengan nilai sebelumnya
#define ADDR_JADWALJAM 10    // array jam [2]
#define ADDR_JADWALMENIT 20  // array menit [2]
#define ADDR_DURASI 30       // array durasi [2]
#define ADDR_PASSWORD 40     // array password [4]

/*Deklarasi Variabel Komponen yang digunakan*/
#define DHTPIN 5
#define DHTTYPE DHT11
#define LED_AUTO 33
#define LED_MANUAL 32
#define MODE_BTN 26
#define BACK_BTN 27
#define UP_BTN 13
#define DOWN_BTN 12
#define OK_BTN 14
#define RELAY_POMPA1 15
#define RELAY_MANUAL 19  // Relay untuk indikator mode manual
#define BTN_MANUAL 25    // Tombol manual pompa

/*Deklarasi Variabel dan Penetapan Fungsi untuk Pembacaan dan Komunikasi NTP*/
DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000);  // GMT+7 WIB, update tiap 60 detik

/*variabel untuk menyimpan data millis dan cek wifi sekali 5 detik*/
unsigned long lastCheck = 0;
const long checkInterval = 5000;  // cek wifi tiap 5 detik

/* koneksi internet sesuaikan dengan wifi dilokasi yang akan kita tempatkan*/
const char* ssid = "Ws-Eletronika";  // ganti sesuai WiFi kamu
const char* pass = "@bpvppdg25";
const char* auth = BLYNK_AUTH_TOKEN;

/*Flag atau Penanda untuk kondisi*/
bool wifiConnected = false;
bool blynkConnected = false;
bool modeManual = false;

/*Variabel untuk menyimpan data suhu*/
float T = 0;

// Password
int password[4] = { 1, 2, 3, 4 };
int inputBuffer[4] = { 0, 0, 0, 0 };
int curDigit = 0;
bool inputPassword = false;

// Blink cursor
bool blinkState = false;
unsigned long lastBlinkTime = 0;
const int blinkInterval = 500;

// Fungsi-fungsi untuk Menu
const char* menuItems[8] = { "Cek Suhu", "Kalib Suhu", "Set Batas Suhu", "Waktu (RTC)",
                             "Kalib RTC", "Set Jadwal P1", "Set Password", "Upload Program" };
int curMenu = 0;
bool inMenu = false;
bool inCekSuhu = false;
bool inKalibSuhu = false;
bool inSetBatasSuhu = false;

// Kalibrasi suhu ±
int kalibSuhu = 0;
bool flag = false;
// Set Batas Suhu
int batasSuhu = 28;
int batasBuffer[2] = { 2, 8 };  //untuk Menyimpan data suhu karena 2 digit
// ---------- Jadwal P1 ----------
int jadwalJam[2] = { 8, 20 };                  // default jam mulai
int jadwalMenit[2] = { 0, 0 };                 // default menit mulai
int durasiMenit[2] = { 1, 1 };                 // durasi aktif relay dalam menit
bool relayOn[2] = { false, false };            // status relay tiap jadwal
unsigned long relayStartMillis[2] = { 0, 0 };  // waktu mulai relay
unsigned long lastBlynkUpdate = 0;
const long blynkInterval = 2000;  // 2 detik
// ---------- WiFi/Blynk reconnect helpers ----------
unsigned long lastWifiTry = 0;
unsigned long lastBlynkTry = 0;
bool pompaManual = false;
bool lastBtnState = HIGH;
static bool synced = false;  //flag untuk menyimpan kondisi sinkronisasi waktu ntp dan rtc

/*Tombol Virtual Blynk*/
BLYNK_WRITE(V0) {
  int v = param.asInt();  // 0 = Auto, 1 = Manual
  modeManual = (v != 0);  // update mode
  digitalWrite(LED_AUTO, !modeManual);
  digitalWrite(LED_MANUAL, modeManual);
  digitalWrite(RELAY_MANUAL, modeManual ? LOW : HIGH);  // Relay indikator manual

  // Kalau tombol fisik ditekan kemudian update Blynk
  Blynk.virtualWrite(V0, modeManual ? 1 : 0);
}

// Tombol Pompa dikondisi manual
BLYNK_WRITE(V2) {
  int v = param.asInt();
  pompaManual = (v != 0);
  digitalWrite(RELAY_POMPA1, pompaManual ? HIGH : LOW);
}

/*Fungsi untuk sinkronisasi RTC dan NTP*/
void syncRTCwithNTP() {
  unsigned long nowMillis = millis();

  if (nowMillis - lastCheck >= checkInterval) {
    lastCheck = nowMillis;


    timeClient.update();
    synced = false;
    if (!synced) {
      unsigned long epochTime = timeClient.getEpochTime();
      rtc.adjust(DateTime(epochTime));
      synced = true;
    }
  }
}


void setup() {

  lcd.init();
  lcd.backlight();
  EEPROM.begin(512);
  loadSettings();  // ambil nilai terakhir dari EEPROM

  dht.begin();
  if (!rtc.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("RTC Error!");
    while (1)
      ;
  }
  pinMode(RELAY_MANUAL, OUTPUT);
  pinMode(BTN_MANUAL, INPUT_PULLUP);  // Tombol aktif LOW
  pinMode(MODE_BTN, INPUT_PULLUP);
  pinMode(LED_AUTO, OUTPUT);
  pinMode(LED_MANUAL, OUTPUT);
  pinMode(BACK_BTN, INPUT_PULLUP);
  pinMode(UP_BTN, INPUT_PULLUP);
  pinMode(DOWN_BTN, INPUT_PULLUP);
  pinMode(OK_BTN, INPUT_PULLUP);
  pinMode(RELAY_POMPA1, OUTPUT);
  digitalWrite(RELAY_MANUAL, HIGH);

  WiFi.begin(ssid, pass);  // Start WiFi
  Blynk.config(auth);      // Prepare Blynk
  unsigned long t0 = millis();
  while (millis() - t0 < 3000 && WiFi.status() != WL_CONNECTED) { delay(200); }
  if (WiFi.status() == WL_CONNECTED) Blynk.connect(2000);
  timeClient.begin();

  Blynk.syncVirtual(V0, !modeManual);

  lcd.setCursor(0, 0);
  lcd.print(" SISTEM START  ");
  lcd.setCursor(0, 1);
  lcd.print("   Loading...  ");
  delay(2000);
  lcd.clear();
}


// ---------- Fungsi update relay & LED ----------
void updateRelayAndLed() {
  digitalWrite(LED_AUTO, !modeManual);
  digitalWrite(LED_MANUAL, modeManual);

  // Kondisi relay berdasarkan suhu
  bool relaySuhu = (T + kalibSuhu >= batasSuhu);

  // Kondisi relay berdasarkan jadwal
  bool relayJadwal = false;
  for (int i = 0; i < 2; i++) {
    if (relayOn[i]) {
      relayJadwal = true;
      // Cek durasi jadwal
      unsigned long elapsed = millis() - relayStartMillis[i];
      if (elapsed >= durasiMenit[i] * 60000UL) {  //ini 60000 = 1 menit, kalau mau detik ubah saja jadi 1000
        relayOn[i] = false;
        relayJadwal = false;
      }
    }
  }

  // Relay nyala kalau suhu > batas atau jadwal aktif
  // digitalWrite(RELAY_POMPA1, relaySuhu || relayJadwal);// "hapus relaySuhu ||" untuk menghilangkan fungsi logika batas suhu
  digitalWrite(RELAY_POMPA1, relayJadwal);
}

void updateJadwalP1() {
  DateTime now = rtc.now();

  for (int i = 0; i < 2; i++) {
    if (now.hour() == jadwalJam[i] && now.minute() == jadwalMenit[i] && !relayOn[i]) {
      relayOn[i] = true;
      relayStartMillis[i] = millis();
    }
  }
}


void menuSetJadwalP1() {
  int curJadwal = 0;  // 0=jadwal1,1=jadwal2
  int curDigitJ = 0;  // 0=jam,1=menit,2=durasi
  bool inJadwal = true;

  while (inJadwal) {
    lcd.setCursor(0, 0);
    lcd.print("Set Jadwal P1:");
    lcd.setCursor(0, 1);
    lcd.print(curJadwal == 0 ? "Jadwal1: " : "Jadwal2: ");

    if (curDigitJ == 0 && blinkState) lcd.print("__");
    else {
      if (jadwalJam[curJadwal] < 10) lcd.print('0');
      lcd.print(jadwalJam[curJadwal]);
    }
    lcd.print(':');
    if (curDigitJ == 1 && blinkState) lcd.print("__");
    else {
      if (jadwalMenit[curJadwal] < 10) lcd.print('0');
      lcd.print(jadwalMenit[curJadwal]);
    }

    lcd.print(' ');
    if (curDigitJ == 2 && blinkState) lcd.print("__");
    else lcd.print(durasiMenit[curJadwal]);

    unsigned long nowTime = millis();
    if (nowTime - lastBlinkTime >= blinkInterval) {
      blinkState = !blinkState;
      lastBlinkTime = nowTime;
    }

    // Tombol UP/DOWN
    if (digitalRead(UP_BTN) == LOW) {
      if (curDigitJ == 0) {
        jadwalJam[curJadwal]++;
        if (jadwalJam[curJadwal] > 23) jadwalJam[curJadwal] = 0;
      } else if (curDigitJ == 1) {
        jadwalMenit[curJadwal]++;
        if (jadwalMenit[curJadwal] > 59) jadwalMenit[curJadwal] = 0;
      } else {
        durasiMenit[curJadwal]++;
        if (durasiMenit[curJadwal] > 59) durasiMenit[curJadwal] = 1;
      }
      delay(150);
    }
    if (digitalRead(DOWN_BTN) == LOW) {
      if (curDigitJ == 0) {
        jadwalJam[curJadwal]--;
        if (jadwalJam[curJadwal] < 0) jadwalJam[curJadwal] = 23;
      } else if (curDigitJ == 1) {
        jadwalMenit[curJadwal]--;
        if (jadwalMenit[curJadwal] < 0) jadwalMenit[curJadwal] = 59;
      } else {
        durasiMenit[curJadwal]--;
        if (durasiMenit[curJadwal] < 1) durasiMenit[curJadwal] = 1;
      }
      delay(150);
    }

    // Tombol OK pindah digit/jadwal
    if (digitalRead(OK_BTN) == LOW) {
      delay(200);
      curDigitJ++;
      if (curDigitJ > 2) {
        curDigitJ = 0;
        curJadwal++;
        if (curJadwal > 1) {  // selesai set 2 jadwal
          inJadwal = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Jadwal Tersimpan");
          delay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">");
          lcd.setCursor(1, 0);
          lcd.print(menuItems[curMenu]);
        }
      }
    }

    if (digitalRead(BACK_BTN) == LOW) {  // batal
      inJadwal = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(">");
      lcd.setCursor(1, 0);
      lcd.print(menuItems[curMenu]);
      delay(200);
    }

    delay(200);
  }
}

// ---------- Fungsi kalibrasi ± ----------
void updateKalibSuhu() {
  if (digitalRead(UP_BTN) == LOW) {
    if (curDigit == 0) kalibSuhu = -kalibSuhu;
    else if (curDigit == 1) {
      int absVal = abs(kalibSuhu);
      int satuan = absVal % 10;
      int puluhan = absVal / 10;
      puluhan++;
      if (puluhan > 9) puluhan = 0;
      kalibSuhu = (kalibSuhu < 0 ? -1 : 1) * (puluhan * 10 + satuan);
    } else if (curDigit == 2) {
      int absVal = abs(kalibSuhu);
      int puluhan = absVal / 10;
      int satuan = absVal % 10;
      satuan++;
      if (satuan > 9) satuan = 0;
      kalibSuhu = (kalibSuhu < 0 ? -1 : 1) * (puluhan * 10 + satuan);
    }
    delay(150);
  }
  if (digitalRead(DOWN_BTN) == LOW) {
    if (curDigit == 0) kalibSuhu = -kalibSuhu;
    else if (curDigit == 1) {
      int absVal = abs(kalibSuhu);
      int satuan = absVal % 10;
      int puluhan = absVal / 10;
      puluhan--;
      if (puluhan < 0) puluhan = 9;
      kalibSuhu = (kalibSuhu < 0 ? -1 : 1) * (puluhan * 10 + satuan);
    } else if (curDigit == 2) {
      int absVal = abs(kalibSuhu);
      int puluhan = absVal / 10;
      int satuan = absVal % 10;
      satuan--;
      if (satuan < 0) satuan = 9;
      kalibSuhu = (kalibSuhu < 0 ? -1 : 1) * (puluhan * 10 + satuan);
    }
    delay(150);
  }
}
// ---------- Fungsi animasi Upload Program ----------
void animasiUpload() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Upload Program");

  for (int i = 0; i < 4; i++) {  // total 2 detik (4 x 500ms)
    lcd.setCursor(0, 1);
    lcd.print("               ");  // clear baris 2
    lcd.setCursor(0, 1);
    for (int j = 0; j <= i; j++) lcd.print('.');
    delay(500);
  }
  saveSettings();
  lcd.setCursor(0, 0);
  lcd.print("Upload Berhasil");
  delay(2000);

  lcd.clear();
}

void updateTombolBlynk() {
  if (modeManual) {
    Blynk.setProperty(V2, "isDisabled", false);  // tombol muncul di mode manual
    Blynk.setProperty(V2, "color", "#00FF00");
  } else {
    Blynk.setProperty(V2, "isDisabled", true);  // tombol hilang di mode auto
    Blynk.setProperty(V2, "color", "#808080");
    pompaManual = false;  // reset relay
                          //------------>>>> // digitalWrite(RELAY_POMPA1, LOW);//this mistake you can hapus ini hahahaha diprogram sblmnya hapusini ..............................
  }
}

void kondisiManual() {
  if (modeManual) {
    digitalWrite(RELAY_MANUAL, LOW);  // Relay manual ON

    static bool lastBtnState = HIGH;
    bool curBtnState = digitalRead(BTN_MANUAL);

    if (curBtnState == LOW && lastBtnState == HIGH) {  // hanya saat tombol ditekan
      pompaManual = !pompaManual;                      // toggle
      Blynk.virtualWrite(V2, pompaManual ? 1 : 0);     // update tombol virtual
    }
    lastBtnState = curBtnState;

    digitalWrite(RELAY_POMPA1, pompaManual ? HIGH : LOW);
  } else {
    digitalWrite(RELAY_MANUAL, HIGH);  // relay manual OFF di mode Auto
    pompaManual = false;
  }

  updateTombolBlynk();  // panggil update tombol di Blynk

  if (!modeManual) {
    Blynk.virtualWrite(V2, 0);
  }
}


// ---------------- WiFi/Blynk reconnect (non-blocking) ----------------
void tryReconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED && millis() - lastWifiTry > 5000) {
    lastWifiTry = millis();
    WiFi.disconnect(true);
    WiFi.begin(ssid, pass);
  }
}

void tryReconnectBlynk() {
  if (WiFi.status() == WL_CONNECTED && !Blynk.connected() && millis() - lastBlynkTry > 2000) {
    lastBlynkTry = millis();
    Blynk.connect();
  }
}

void loop() {
  // Jalankan Blynk hanya kalau koneksi ada, tapi jangan blokir program
  tryReconnectWiFi();
  tryReconnectBlynk();


  // kode sekali jalan


  // delay(1000);

  // if (WiFi.status() == WL_CONNECTED && Blynk.connected()){ Blynk.run();}  // Run Blynk
  if (WiFi.status() == WL_CONNECTED && Blynk.connected()) {
    // Blynk.syncVirtual(V0,modeManual ? 1 : 0);
    syncRTCwithNTP();  // cek koneksi + sinkronisasi RTC ke NTP

    if (flag == false) {
      Blynk.syncVirtual(V0, modeManual ? 1 : 0);
      lcd.init();
      lcd.backlight();
      inMenu = false;  // keluar dari menu
      flag = true;
    }
    Blynk.run();
    Seluruhnya();

    // Kirim ke Blynk cuma setiap 5 detik
    if (millis() - lastBlynkUpdate > blynkInterval) {
      if (Blynk.connected()) {
        Blynk.virtualWrite(V1, T + kalibSuhu);
      }
      lastBlynkUpdate = millis();
    }
  } else {
    if (flag == true) {
      lcd.init();
      lcd.backlight();
      inMenu = false;  // keluar dari menu
      flag = false;
    }
    Seluruhnya();
    synced = false;
  }
}

void Seluruhnya() {
  T = dht.readTemperature();
  DateTime now = rtc.now();

  // Update jadwal dulu
  updateJadwalP1();

  // Update relay & LED gabungan
  updateRelayAndLed();
  kondisiManual();

  // ---------- Input Password ----------
  if (inputPassword) {
    unsigned long nowTime = millis();
    if (nowTime - lastBlinkTime >= blinkInterval) {
      blinkState = !blinkState;
      lastBlinkTime = nowTime;
      lcd.setCursor(curDigit * 2, 1);
      if (blinkState) lcd.print('_');
      else lcd.print(inputBuffer[curDigit]);
    }

    if (digitalRead(UP_BTN) == LOW) {
      inputBuffer[curDigit]++;
      if (inputBuffer[curDigit] > 9) inputBuffer[curDigit] = 0;
      lcd.setCursor(curDigit * 2, 1);
      lcd.print(inputBuffer[curDigit]);
      delay(150);
    }
    if (digitalRead(DOWN_BTN) == LOW) {
      inputBuffer[curDigit]--;
      if (inputBuffer[curDigit] < 0) inputBuffer[curDigit] = 9;
      lcd.setCursor(curDigit * 2, 1);
      lcd.print(inputBuffer[curDigit]);
      delay(150);
    }

    if (digitalRead(OK_BTN) == LOW) {
      delay(200);
      curDigit++;
      if (curDigit > 3) {
        bool passOk = true;
        for (int i = 0; i < 4; i++)
          if (inputBuffer[i] != password[i]) passOk = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        if (passOk) {
          lcd.print("Password OK");
          delay(500);
          inMenu = true;
          curMenu = 0;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">");
          lcd.setCursor(1, 0);
          lcd.print(menuItems[curMenu]);
          inputPassword = false;
        } else {
          lcd.print("Password Salah");
          delay(1000);
          for (int i = 0; i < 4; i++) inputBuffer[i] = 0;
          curDigit = 0;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Password:");
          for (int i = 0; i < 4; i++) {
            lcd.setCursor(i * 2, 1);
            lcd.print(inputBuffer[i]);
          }
          lastBlinkTime = millis();
        }
      } else lcd.setCursor(curDigit * 2, 1);
    }

    // ---------- Menu utama ----------
  } else if (inMenu) {
    if (!inCekSuhu && !inKalibSuhu && !inSetBatasSuhu) {
      if (digitalRead(UP_BTN) == LOW) {
        curMenu--;
        if (curMenu < 0) curMenu = 7;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(">");
        lcd.setCursor(1, 0);
        lcd.print(menuItems[curMenu]);
        delay(200);
      }
      if (digitalRead(DOWN_BTN) == LOW) {
        curMenu++;
        if (curMenu > 7) curMenu = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(">");
        lcd.setCursor(1, 0);
        lcd.print(menuItems[curMenu]);
        delay(200);
      }
      if (digitalRead(OK_BTN) == LOW) {
        delay(200);
        if (curMenu == 0) {
          inCekSuhu = true;
          lcd.clear();
        } else if (curMenu == 1) {
          inKalibSuhu = true;
          curDigit = 0;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Kalib Suhu:");
          lastBlinkTime = millis();
        } else if (curMenu == 2) {
          inSetBatasSuhu = true;
          curDigit = 0;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Set Batas Suhu:");

          // Ambil nilai terakhir ke buffer digit
          batasBuffer[0] = batasSuhu / 10;  // puluhan
          batasBuffer[1] = batasSuhu % 10;  // satuan

          lcd.setCursor(0, 1);
          lcd.print(batasBuffer[0]);
          lcd.print(batasBuffer[1]);

          lastBlinkTime = millis();
        } else if (curMenu == 3) {  // Sub-menu Waktu (RTC)
          lcd.clear();
          bool inWaktu = true;
          while (inWaktu) {
            DateTime now = rtc.now();
            // Baris 0: Hari + Tanggal
            lcd.setCursor(0, 0);
            switch (now.dayOfTheWeek()) {
              case 0: lcd.print("Minggu "); break;
              case 1: lcd.print("Senin "); break;
              case 2: lcd.print("Selasa "); break;
              case 3: lcd.print("Rabu "); break;
              case 4: lcd.print("Kamis "); break;
              case 5: lcd.print("Jumat "); break;
              case 6: lcd.print("Sabtu "); break;
            }
            if (now.day() < 10) lcd.print('0');
            lcd.print(now.day());
            lcd.print('/');
            if (now.month() < 10) lcd.print('0');
            lcd.print(now.month());
            lcd.print('/');
            lcd.print(now.year());

            // Baris 1: Jam:Menit:Detik
            lcd.setCursor(0, 1);
            if (now.hour() < 10) lcd.print('0');
            lcd.print(now.hour());
            lcd.print(':');
            if (now.minute() < 10) lcd.print('0');
            lcd.print(now.minute());
            lcd.print(':');
            if (now.second() < 10) lcd.print('0');
            lcd.print(now.second());

            if (digitalRead(BACK_BTN) == LOW) {
              inWaktu = false;
              delay(200);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print(">");
              lcd.setCursor(1, 0);
              lcd.print(menuItems[curMenu]);
            }
            delay(200);
          }
        } else if (curMenu == 4) {  // Sub-menu Kalib RTC
          lcd.clear();
          if (WiFi.status() != WL_CONNECTED) {
            int jam = now.hour();
            int menit = now.minute();
            int detik = now.second();
            int curDigitRTC = 0;  // 0=jam,1=menit,2=detik
            bool inKalibRTC = true;

            while (inKalibRTC) {
              // Baris 0: keterangan
              lcd.setCursor(0, 0);
              lcd.print("Kalib RTC:");

              // Baris 1: tampil jam:menit:detik dengan cursor kedip
              lcd.setCursor(0, 1);
              if (curDigitRTC == 0 && blinkState) lcd.print("__");
              else {
                if (jam < 10) lcd.print('0');
                lcd.print(jam);
              }
              lcd.print(':');
              if (curDigitRTC == 1 && blinkState) lcd.print("__");
              else {
                if (menit < 10) lcd.print('0');
                lcd.print(menit);
              }
              lcd.print(':');
              if (curDigitRTC == 2 && blinkState) lcd.print("__");
              else {
                if (detik < 10) lcd.print('0');
                lcd.print(detik);
              }

              // Update blink
              unsigned long nowTime = millis();
              if (nowTime - lastBlinkTime >= blinkInterval) {
                blinkState = !blinkState;
                lastBlinkTime = nowTime;
              }

              // Tombol UP/DOWN
              if (digitalRead(UP_BTN) == LOW) {
                if (curDigitRTC == 0) {
                  jam++;
                  if (jam > 23) jam = 0;
                } else if (curDigitRTC == 1) {
                  menit++;
                  if (menit > 59) menit = 0;
                } else if (curDigitRTC == 2) {
                  detik++;
                  if (detik > 59) detik = 0;
                }
                delay(150);
              }
              if (digitalRead(DOWN_BTN) == LOW) {
                if (curDigitRTC == 0) {
                  jam--;
                  if (jam < 0) jam = 23;
                } else if (curDigitRTC == 1) {
                  menit--;
                  if (menit < 0) menit = 59;
                } else if (curDigitRTC == 2) {
                  detik--;
                  if (detik < 0) detik = 59;
                }
                delay(150);
              }

              // Tombol OK pindah digit
              if (digitalRead(OK_BTN) == LOW) {
                delay(200);
                curDigitRTC++;
                if (curDigitRTC > 2) {  // Selesai kalibrasi
                  rtc.adjust(DateTime(now.year(), now.month(), now.day(), jam, menit, detik));
                  inKalibRTC = false;
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print("Simpan OK");
                  delay(500);
                  lcd.clear();
                  lcd.setCursor(0, 0);
                  lcd.print(">");
                  lcd.setCursor(1, 0);
                  lcd.print(menuItems[curMenu]);
                }
              }

              // Tombol BACK batal kalibrasi
              if (digitalRead(BACK_BTN) == LOW) {
                inKalibRTC = false;
                delay(200);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(">");
                lcd.setCursor(1, 0);
                lcd.print(menuItems[curMenu]);
              }

              delay(200);
            }
          } else {
            lcd.setCursor(0, 0);
            lcd.print("Anda Terhubung");
            lcd.setCursor(0, 1);
            lcd.print("WiFi !!!!!!!!!");
            delay(2000);
            inMenu = false;  // keluar dari menu
            lcd.clear();     // balik ke tampilan awal
          }
        } else if (curMenu == 5) {
          lcd.clear();
          menuSetJadwalP1();
        } else if (curMenu == 6) {  // Sub-menu Set Password
          lcd.clear();
          int newPass[4] = { 0, 0, 0, 0 };
          int curDigitPass = 0;
          bool inSetPass = true;
          lastBlinkTime = millis();
          blinkState = false;

          while (inSetPass) {
            lcd.setCursor(0, 0);
            lcd.print("Set Password:");

            // Tampilkan password dengan cursor berkedip
            for (int i = 0; i < 4; i++) {
              lcd.setCursor(i * 2, 1);
              if (i == curDigitPass && blinkState) lcd.print('_');
              else lcd.print(newPass[i]);
            }

            // Blink update
            unsigned long nowTime = millis();
            if (nowTime - lastBlinkTime >= blinkInterval) {
              blinkState = !blinkState;
              lastBlinkTime = nowTime;
            }

            // Tombol UP
            if (digitalRead(UP_BTN) == LOW) {
              newPass[curDigitPass]++;
              if (newPass[curDigitPass] > 9) newPass[curDigitPass] = 0;
              delay(150);
            }
            // Tombol DOWN
            if (digitalRead(DOWN_BTN) == LOW) {
              newPass[curDigitPass]--;
              if (newPass[curDigitPass] < 0) newPass[curDigitPass] = 9;
              delay(150);
            }
            // Tombol OK
            if (digitalRead(OK_BTN) == LOW) {
              delay(200);
              curDigitPass++;
              if (curDigitPass > 3) {
                // Simpan password baru
                for (int i = 0; i < 4; i++) password[i] = newPass[i];
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Password Tersimpan");
                delay(1000);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print(">");
                lcd.setCursor(1, 0);
                lcd.print(menuItems[curMenu]);
                inSetPass = false;
              }
            }
            // Tombol BACK
            if (digitalRead(BACK_BTN) == LOW) {
              delay(200);
              inSetPass = false;
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print(">");
              lcd.setCursor(1, 0);
              lcd.print(menuItems[curMenu]);
            }
          }
        } else if (curMenu == 7) {  // Sub-menu Upload Program
          animasiUpload();
          inMenu = false;  // keluar dari menu
          lcd.clear();     // balik ke tampilan awal
        }
      }
      if (digitalRead(BACK_BTN) == LOW) {
        inMenu = false;
        lcd.clear();
      }
    }


    // ---------- Sub-menu Cek Suhu ----------
    else if (inCekSuhu) {
      lcd.setCursor(0, 0);
      lcd.print("Suhu: ");
      float displayT = T + kalibSuhu;
      if (isnan(displayT)) lcd.print("Err ");
      else lcd.print(displayT);
      lcd.print((char)223);
      lcd.print("C");

      if (digitalRead(BACK_BTN) == LOW) {
        inCekSuhu = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(">");
        lcd.setCursor(1, 0);
        lcd.print(menuItems[curMenu]);
        delay(200);
      }
    }

    // ---------- Sub-menu Kalib Suhu ----------
    else if (inKalibSuhu) {
      unsigned long nowTime = millis();
      if (nowTime - lastBlinkTime >= blinkInterval) {
        blinkState = !blinkState;
        lastBlinkTime = nowTime;
        // ± dan dua digit
        lcd.setCursor(0, 1);
        if (curDigit == 0 && blinkState) lcd.print('_');
        else lcd.print(kalibSuhu < 0 ? '-' : '+');
        int absVal = abs(kalibSuhu);
        int puluhan = absVal / 10;
        int satuan = absVal % 10;
        lcd.setCursor(1, 1);
        if (curDigit == 1 && blinkState) lcd.print('_');
        else lcd.print(puluhan);
        lcd.setCursor(2, 1);
        if (curDigit == 2 && blinkState) lcd.print('_');
        else lcd.print(satuan);
      }

      // Tombol UP/DOWN untuk ±
      updateKalibSuhu();

      if (digitalRead(OK_BTN) == LOW) {
        delay(200);
        curDigit++;
        if (curDigit > 2) {
          inKalibSuhu = false;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Simpan OK");
          delay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">");
          lcd.setCursor(1, 0);
          lcd.print(menuItems[curMenu]);
        }
      }

      if (digitalRead(BACK_BTN) == LOW) {
        inKalibSuhu = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(">");
        lcd.setCursor(1, 0);
        lcd.print(menuItems[curMenu]);
        delay(200);
      }
    }

    // ---------- Sub-menu Set Batas Suhu ----------
    else if (inSetBatasSuhu) {
      unsigned long nowTime = millis();
      if (nowTime - lastBlinkTime >= blinkInterval) {
        blinkState = !blinkState;
        lastBlinkTime = nowTime;
        lcd.setCursor(curDigit, 1);
        if (blinkState) lcd.print('_');
        else lcd.print(batasBuffer[curDigit]);
      }

      if (digitalRead(UP_BTN) == LOW) {
        batasBuffer[curDigit]++;
        if (batasBuffer[curDigit] > 9) batasBuffer[curDigit] = 0;
        lcd.setCursor(curDigit, 1);
        lcd.print(batasBuffer[curDigit]);
        delay(150);
      }
      if (digitalRead(DOWN_BTN) == LOW) {
        batasBuffer[curDigit]--;
        if (batasBuffer[curDigit] < 0) batasBuffer[curDigit] = 9;
        lcd.setCursor(curDigit, 1);
        lcd.print(batasBuffer[curDigit]);
        delay(150);
      }

      if (digitalRead(OK_BTN) == LOW) {
        delay(200);
        curDigit++;
        if (curDigit > 1) {
          inSetBatasSuhu = false;
          batasSuhu = batasBuffer[0] * 10 + batasBuffer[1];
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Simpan OK");
          delay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(">");
          lcd.setCursor(1, 0);
          lcd.print(menuItems[curMenu]);
        } else lcd.setCursor(curDigit, 1);
      }

      if (digitalRead(BACK_BTN) == LOW) {
        inSetBatasSuhu = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(">");
        lcd.setCursor(1, 0);
        lcd.print(menuItems[curMenu]);
        delay(200);
      }
    }


    // ---------- Tampilan Awal ----------
  } else {
    lcd.setCursor(0, 0);
    lcd.print("T:");
    float displayT = T + kalibSuhu;
    DateTime now = rtc.now();
    if (isnan(displayT)) lcd.print("Err ");
    else lcd.print(displayT);
    lcd.print((char)223);
    lcd.print("C ");
    lcd.setCursor(10, 0);
    lcd.print(modeManual ? "Manual" : "  Auto");

    lcd.setCursor(0, 1);
    if (now.day() < 10) lcd.print('0');
    lcd.print(now.day());
    lcd.print('/');
    if (now.month() < 10) lcd.print('0');
    lcd.print(now.month());
    lcd.print("   ");
    if (now.hour() < 10) lcd.print('0');
    lcd.print(now.hour());
    lcd.print(':');
    if (now.minute() < 10) lcd.print('0');
    lcd.print(now.minute());
    lcd.print(':');
    if (now.second() < 10) lcd.print('0');
    lcd.print(now.second());

    if (digitalRead(BACK_BTN) == LOW) {
      delay(200);
      inputPassword = true;
      curDigit = 0;
      for (int i = 0; i < 4; i++) inputBuffer[i] = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Password:");
      for (int i = 0; i < 4; i++) {
        lcd.setCursor(i * 2, 1);
        lcd.print(inputBuffer[i]);
      }
      lastBlinkTime = millis();
    }
  }

  // Tombol mode Auto/Manual
  // Tombol mode Auto/Manual fisik
  if (digitalRead(MODE_BTN) == LOW) {
    modeManual = !modeManual;  // toggle mode
    digitalWrite(LED_AUTO, !modeManual);
    digitalWrite(LED_MANUAL, modeManual);
    digitalWrite(RELAY_MANUAL, modeManual ? LOW : HIGH);

    // update Blynk juga
    if (Blynk.connected()) {
      Blynk.virtualWrite(V0, modeManual ? 1 : 0);
    }
    delay(300);  // debounce
  }


  delay(200);
}

// ----------- SIMPAN DATA KE EEPROM -----------
void saveSettings() {
  EEPROM.put(ADDR_KALIBSUHU, kalibSuhu);
  EEPROM.put(ADDR_BATASSUHU, batasSuhu);
  EEPROM.put(ADDR_JADWALJAM, jadwalJam);
  EEPROM.put(ADDR_JADWALMENIT, jadwalMenit);
  EEPROM.put(ADDR_DURASI, durasiMenit);
  EEPROM.put(ADDR_PASSWORD, password);

  EEPROM.commit();
}

// ----------- BACA DATA DARI EEPROM -----------
void loadSettings() {
  EEPROM.get(ADDR_KALIBSUHU, kalibSuhu);
  EEPROM.get(ADDR_BATASSUHU, batasSuhu);
  EEPROM.get(ADDR_JADWALJAM, jadwalJam);
  EEPROM.get(ADDR_JADWALMENIT, jadwalMenit);
  EEPROM.get(ADDR_DURASI, durasiMenit);
  EEPROM.get(ADDR_PASSWORD, password);

  // Cek validasi (jaga-jaga kalau EEPROM kosong)
  if (batasSuhu <= 0 || batasSuhu > 99) batasSuhu = 28;  // default
  if (kalibSuhu < -50 || kalibSuhu > 50) kalibSuhu = 0;  // default
  for (int i = 0; i < 2; i++) {
    if (jadwalJam[i] < 0 || jadwalJam[i] > 23) jadwalJam[i] = (i == 0 ? 8 : 20);
    if (jadwalMenit[i] < 0 || jadwalMenit[i] > 59) jadwalMenit[i] = 0;
    if (durasiMenit[i] <= 0 || durasiMenit[i] > 59) durasiMenit[i] = 1;
  }
  for (int i = 0; i < 4; i++) {
    if (password[i] < 0 || password[i] > 9) password[i] = i + 1;  // default 1234
  }
}
