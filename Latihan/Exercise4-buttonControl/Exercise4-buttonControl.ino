const int bt1 = 27;
const int bt2 = 14;
const int ledPin = 26;
const int soilPin = 33;

void setup() {
  Serial.begin(9600);
  pinMode(bt1, INPUT_PULLUP);
  pinMode(bt2, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  if (digitalRead(bt1) == LOW) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  if (digitalRead(bt2) == LOW) {
    int humidityValue = analogRead(soilPin);
    Serial.println(humidityValue);
  }
}
