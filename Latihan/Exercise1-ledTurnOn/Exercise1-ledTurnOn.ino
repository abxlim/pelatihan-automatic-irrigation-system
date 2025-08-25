const int bt1 = 13;
const int bt2 = 12;
const int bt3 = 14;
const int lamp = 26;

bool state = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(bt1, INPUT_PULLUP);
  pinMode(bt2, INPUT_PULLUP);
  pinMode(bt3, INPUT_PULLUP);
  pinMode(lamp, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (digitalRead(bt1) == LOW) {
    state = true;
  } else if (digitalRead(bt2) == LOW) {
    state = true;
  } else if (digitalRead(bt3) == LOW) {
    state = true;
  } else {
    state = false;
  }

  digitalWrite(lamp, state);
}
