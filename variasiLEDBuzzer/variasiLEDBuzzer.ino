// Pin untuk LED RGB
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;
const int step = 5;
const int waktu = 5;

const int buzz = 6;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzz, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  for (int red = 0; red <= 255; red += step) {
    analogWrite(redPin, red);
    analogWrite(greenPin, 255 - red);
    int freq = map(red, 0, 255, 500, 1000);
    tone(buzz, freq);
    delay(waktu);
  }
  for (int green = 0; green <= 255; green += step) {
    analogWrite(greenPin, green);
    analogWrite(bluePin, 255 - green);
    int freq = map(255 - green, 0, 255, 500, 1000);
    tone(buzz, freq);
    delay(waktu);
  }
}
