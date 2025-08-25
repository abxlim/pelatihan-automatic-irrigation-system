const int resistiveSoilPin = 33;
const int relayPin = 19;
const int ledPin = 26;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(relayPin,OUTPUT);
  pinMode(ledPin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int nilaiADC = analogRead(resistiveSoilPin);
  // int humidityPercent = map(nilaiADC, 4095, 1630, 0, 100);
  // humidityPercent = constrain(humidityPercent, 0, 100);

  Serial.println(nilaiADC);

  if (nilaiADC > 3000) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Relay Aktif");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("Relay Mati");
    delay(100);
  }
}
