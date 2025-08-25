int LED1 = 9;
int LED2 = 10;
int LED3 = 11;
int waktu = 1000;
int Buzzer = 6;

void setup() {
  // put your setup code here, to run once:

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(Buzzer, OUTPUT);
}

void merah() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
}

void hijau() {
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, HIGH);
}

void biru() {
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, LOW);
}

void ungu() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, LOW);
}

void kuning() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, HIGH);
}

void cyan() {
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  merah();
  delay(waktu);
  biru();
  digitalWrite(Buzzer, HIGH);
  delay(waktu);
  digitalWrite(Buzzer, LOW);
  hijau();
  delay(waktu);
  ungu();
  digitalWrite(Buzzer, HIGH);
  delay(waktu);
  digitalWrite(Buzzer, LOW);
  kuning();
  delay(waktu);
  cyan();
  digitalWrite(Buzzer, HIGH);
  delay(waktu);
  digitalWrite(Buzzer, LOW);
}
