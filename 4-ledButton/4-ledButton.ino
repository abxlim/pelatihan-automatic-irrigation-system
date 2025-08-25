const int LEDPin = 2;
const int buttonPin = 4;

int ledState = LOW;  // the current state of the output pin
int buttonState;     // the current reading from the input pin
int incState = 0;
int lastButtonState = LOW;  // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long lastMillisTime = 0;

int timeDelay = 2000;

void setup() {
  // put your setup code here, to run once:

  pinMode(LEDPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {

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
      }
    }
  }

  lastButtonState = reading;

  if (millis() - lastMillisTime >= timeDelay) {
    digitalWrite(LEDPin, ledState);
      ledState = !ledState;
      
      lastMillisTime = millis();
  }

  if (incState == 1) {
    timeDelay = 1000;
  } else if (incState == 2) {
    timeDelay = 500;
  }
}

// kondisi 1, kiri kana, 2, kanan kiri, 3 tengnah buka, 4 luar ke tengah
