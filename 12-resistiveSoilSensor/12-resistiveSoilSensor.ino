const int soilResistPin = 25;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int nilaiADC = analogRead(soilResistPin);
  int soilPercent = map(nilaiADC, 4095, 1630, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);
  Serial.print(soilPercent);
  Serial.println("%");
  delay(100);
}
