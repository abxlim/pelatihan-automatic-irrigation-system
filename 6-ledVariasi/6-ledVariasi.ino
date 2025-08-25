int ledPins[] = { 15, 2, 5, 18, 19, 21, 22 };  // LED terhubung ke pin digital 2, 3, 4, dan 5
int jumlahLED = 7;                             // Jumlah LED

const int buttonPin = 4;

int ledState = LOW;  // the current state of the output pin
int buttonState;     // the current reading from the input pin
int incState = 0;
int lastButtonState = LOW;  // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long lastMillisTime = 0;

int incrementLED = 0;

void setup() {

  for (int i = 0; i < jumlahLED; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  buttonHandling();

  if (millis() - lastMillisTime >= 100) {

    LEDClear();
    LEDVariasi();

    lastMillisTime = millis();
  }
}

void buttonHandling() {
  int reading = digitalRead(buttonPin);
  Serial.println(reading);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        incState++;
        incrementLED = 0;
      }

      if (incState > 3) incState = 0;
    }
  }

  lastButtonState = reading;
}

void LEDClear() {
  for (int i = 0; i < jumlahLED; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

void LEDVariasi() {
  if (incState == 1) {
    digitalWrite(ledPins[incrementLED], HIGH);

    incrementLED++;

    if (incrementLED > jumlahLED) incrementLED = 0;

  } else if (incState == 2) {
    digitalWrite(ledPins[jumlahLED - 1 - incrementLED], HIGH);

    incrementLED++;
    if (incrementLED > jumlahLED) incrementLED = 0;
  } else if (incState == 3){

    int tengah = jumlahLED / 2;
    digitalWrite(ledPins[tengah - incrementLED], HIGH);
    digitalWrite(ledPins[tengah + incrementLED], HIGH);

    incrementLED++;
    if (incrementLED > tengah) incrementLED = 0;
  }
}
