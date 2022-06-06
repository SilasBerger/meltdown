// === Messaging protocol constants ===
const String INPUT_TYPE_STRING = "string";
const String INPUT_TYPE_INTEGER = "int";
const char START_BYTE = (char) 2;
const char END_BYTE = (char) 4;

// === Device names ===
const String DEVICE_NAME_KEYPAD = "keypad";
const String DEVICE_NAME_PREFIX_POT = "pot";
const String DEVICE_NAME_BUTTONS = "buttons";

// ==== Keypad settings ===
const int NUM_ROWS = 4;
const int NUM_COLS = 4;
const unsigned long KEYPAD_DEBOUNCE_DELAY_MS = 50;

// === Pot settings ===
const int POT_CHANGE_MARGIN = 10;
const int NUM_POTS = 2;

// === Button settings ===
const int NUM_BUTTONS = 4;
const unsigned long BUTTONS_DEBOUNCE_DELAY_MS = 50;

// === Pinout ===
byte rowPins[NUM_ROWS] = {9, 8, 7, 6};
byte colPins[NUM_COLS] = {5, 4, 3, 2};
byte potPins[NUM_POTS] = {A0, A1};
byte buttonPins[NUM_BUTTONS] = {A2, A3, A4, A5};

// === Keymap ===
char keys[NUM_ROWS][NUM_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// === Goobal variables ===
unsigned long timestampLastKeypadPress = 0;
int lastPotValues[NUM_POTS];
unsigned long timestampsLastButtonPresses[NUM_BUTTONS];

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");

  initKeypad();
  initPots();
  initButtons();

  Serial.println("Ready!");
}

void initKeypad() {
  for (int row = 0; row < NUM_ROWS; row++) {
    pinMode(rowPins[row], INPUT);
  }

  for (int col = 0; col < NUM_COLS; col++) {
    pinMode(colPins[col], INPUT_PULLUP);
  }
}

void initPots() {
  for (int index; index < NUM_POTS; index++) {
    lastPotValues[index] = -1;
  }
}

void initButtons() {
  for (int index; index < NUM_BUTTONS; index++) {
    pinMode(buttonPins[index], INPUT_PULLUP);
    timestampsLastButtonPresses[index] = 0;
  }
}

void loop() {
  readKeypadInput();
  readPotInputs();
  readButtonInputs();
}

void readKeypadInput() {
  bool buttonPressed = false;
  for (int row = 0; row < NUM_ROWS; row++) {
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], LOW);
    for (int col = 0; col < NUM_COLS; col++) {
      if (digitalRead(colPins[col]) == LOW) {
        unsigned long timestamp = millis();
        unsigned long timeSinceLastButtonPress = timestamp - timestampLastKeypadPress;
        if (timeSinceLastButtonPress > KEYPAD_DEBOUNCE_DELAY_MS) {
          reportKeypadPress(row, col);  
        }
        timestampLastKeypadPress = timestamp;
      }
    }
    pinMode(rowPins[row], INPUT);
  }
}

void readPotInputs() {
  for (int potIndex = 0; potIndex < NUM_POTS; potIndex++) {
    int potValue = analogRead(potPins[potIndex]);
    if (!isEqualWithinMargin(potValue, lastPotValues[potIndex], POT_CHANGE_MARGIN)) {
      reportPotChange(potIndex, potValue);
      lastPotValues[potIndex] = potValue;
    }
  }
}

void readButtonInputs() {
  for (int btnIndex = 0; btnIndex < NUM_BUTTONS; btnIndex++) {
    if (digitalRead(buttonPins[btnIndex]) == LOW) {
      unsigned long timestamp = millis();
      unsigned long timeSinceLastPress = timestamp - timestampsLastButtonPresses[btnIndex];
      if (timeSinceLastPress > BUTTONS_DEBOUNCE_DELAY_MS) {
        reportButtonPress(btnIndex);
      }
      timestampsLastButtonPresses[btnIndex] = timestamp;
    }
  }
}

void reportPotChange(int potIndex, int potValue) {
  sendIntegerInputEvent(DEVICE_NAME_PREFIX_POT + String(potIndex), potValue);
}

void reportKeypadPress(int row, int col) {
  sendStringInputEvent(DEVICE_NAME_KEYPAD, String(keys[row][col]));
}

void reportButtonPress(int btnIndex) {
  sendIntegerInputEvent(DEVICE_NAME_BUTTONS, btnIndex);
}

bool isEqualWithinMargin(int a, int b, int margin) {
  return abs(a - b) <= margin;
}

void sendStringInputEvent(String deviceName, String value) {
    sendMessage("{\"device\": \"" + deviceName + "\", \"inputType\": \"" + INPUT_TYPE_STRING + "\", \"value\": \"" + value + "\"}");
}

void sendIntegerInputEvent(String deviceName, int value) {
    sendMessage("{\"device\": \"" + deviceName + "\", \"inputType\": \"" + INPUT_TYPE_INTEGER + "\", \"value\": " + value + "}");
}

void sendMessage(String body) {
  Serial.print(START_BYTE);
  Serial.print(body);
  Serial.print(END_BYTE);
}
