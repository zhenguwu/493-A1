const int RED_PIN = 11;
const int GREEN_PIN = 5;
const int BLUE_PIN = 3;
const int BUTTON = 2;
const int PHOTOCELL_PIN = A4;
const int GRAPHITE_PIN = A5;
const int TRIG_PIN = 7;
const int ECHO_PIN = 6;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(PHOTOCELL_PIN, INPUT);
  pinMode(GRAPHITE_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(9600);
}

int mode = 1;
void loop() {
  if (buttonPressed()) {
    mode = (mode + 1) % 3;
  }

  switch(mode) {
    case 0:
      crossFade();
      break;
    case 1:
      lofiInput();
      break;
    case 2:
      creative();
      break;
  }
}

// Function that checks if the button is pressed with debouncing
unsigned long lastReadTime = 0;
bool buttonPressed() {
  if(!digitalRead(BUTTON)) {
    if ((millis() - lastReadTime) > 500) {
      lastReadTime = millis();
      return true;
    }
  }
  return false;
}

enum RGB{
  RED,
  GREEN,
  BLUE
};
int rgbVals[] = {255, 0, 0};
enum RGB fadeUp = GREEN;
enum RGB fadeDown = RED;

void crossFade() {
  rgbVals[fadeUp] += 5;
  rgbVals[fadeDown] -= 5;
  if (rgbVals[fadeUp] == 255) {
    // Switch to next color to fade up/down
    fadeUp = (RGB)((int)fadeUp + 1);
    if (fadeUp > (int)BLUE) {
      fadeUp = RED;
    }
    fadeDown = (RGB)((int)fadeDown + 1);
    if (fadeDown > (int)BLUE) {
      fadeDown = RED;
    }
  }

  // Read photocell and scale to 0-255
  int photocellVal = analogRead(PHOTOCELL_PIN);
  int ledVal = 255 - constrain(map(photocellVal, 200, 800, 0, 255), 0, 255);

  // Write RGB values to led scaled by brightness
  analogWrite(RED_PIN, (ledVal * rgbVals[RED]) / 255);
  analogWrite(GREEN_PIN, (ledVal * rgbVals[GREEN]) / 255);
  analogWrite(BLUE_PIN, (ledVal * rgbVals[BLUE]) / 255);
  delay(20);
}

int state = 0; // 0 = red, 1 = green, 2 = blue
void lofiInput() {
  int graphiteVal = analogRead(GRAPHITE_PIN);
  // Serial.println(graphiteVal);

  // Update state if reading is not 0
  if (graphiteVal > 5) {
    if (graphiteVal < 200) {
      state = 0;
    } else if (graphiteVal < 400) {
      state = 1;
    } else {
      state = 2;
    }
  }

  switch(state) {
    case 0:
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, LOW);
      break;
    case 1:
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
      digitalWrite(BLUE_PIN, LOW);
      break;
    case 2:
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(BLUE_PIN, HIGH);
      break;
  }
}

bool nightlight = false;
unsigned long timer = 0;
void creative() {
  if (nightlight) {
    // Was already triggered, need to check if timer is up
    analogWrite(RED_PIN, 255);
    analogWrite(GREEN_PIN, 255);
    analogWrite(BLUE_PIN, 100);
    if (millis() - timer > 10000) {
      nightlight = false;
    }
  } else {
    analogWrite(RED_PIN, 50);
    analogWrite(GREEN_PIN, 10);
    analogWrite(BLUE_PIN, 0);
   
    // Check distance
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    int time = pulseIn(ECHO_PIN, HIGH);
    int dist = time * 0.034 / 2;
    if (dist < 10) {
      // Activate nightlight
      nightlight = true;
      timer = millis();
    }
  }
}