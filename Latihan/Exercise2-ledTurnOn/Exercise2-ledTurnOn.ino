const int bt1 = 13;
const int bt2 = 12;
const int lamp = 26;

bool state = false;
bool stateBtn2 = LOW;

void setup() {
  // put your setup code here, to run once:
  pinMode(bt1, INPUT_PULLUP);
  pinMode(bt2, INPUT_PULLUP);
  pinMode(lamp, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (digitalRead(bt1) == LOW) {
    digitalWrite(lamp, HIGH);
  } else {
    digitalWrite(lamp, state);
  }

  if (digitalRead(bt2) == LOW && stateBtn2 == HIGH) {
    state = !state;
  }

  stateBtn2 = digitalRead(bt2);
  delay(20);
}
