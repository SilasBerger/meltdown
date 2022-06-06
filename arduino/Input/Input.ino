const int NUM_ROWS = 4;
const int NUM_COLS = 4;
const unsigned long DEBOUNCE_DELAY_MS = 50;
const int POT_MARGIN = 10;

const String DEVICE_NAME_KEYPAD = "keypad";
const String DEVICE_NAME_POT_1 = "pot1";
const String INPUT_TYPE_STRING = "string";
const String INPUT_TYPE_INTEGER = "int";
const char START_BYTE = (char) 2;
const char END_BYTE = (char) 4;

char keys[NUM_ROWS][NUM_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[NUM_ROWS] = {9, 8, 7, 6};
byte colPins[NUM_COLS] = {5, 4, 3, 2};

unsigned long timestampLastButtonPress = 0;

byte pot1Pin = A0;
int lastValuePot1 = -1;

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");

  for (int row = 0; row < NUM_ROWS; row++) {
    pinMode(rowPins[row], INPUT);
  }

  for (int col = 0; col < NUM_COLS; col++) {
    pinMode(colPins[col], INPUT_PULLUP);
  }

  Serial.println("Ready!");
}

void loop() {
  readKeypadInput();
  readPot1Input();
}

void readKeypadInput() {
  bool buttonPressed = false;
  for (int row = 0; row < NUM_ROWS; row++) {
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], LOW);
    for (int col = 0; col < NUM_COLS; col++) {
      if (digitalRead(colPins[col]) == LOW) {
        unsigned long timestamp = millis();
        unsigned long timeSinceLastButtonPress = timestamp - timestampLastButtonPress;
        if (timeSinceLastButtonPress > DEBOUNCE_DELAY_MS) {
          reportButtonPress(row, col);  
        }
        timestampLastButtonPress = timestamp;
      }
    }
    pinMode(rowPins[row], INPUT);
  }
}

void readPot1Input() {
  int potValue = analogRead(pot1Pin);
  if (!isEqualWithinMargin(potValue, lastValuePot1, POT_MARGIN)) {
    sendIntegerInputEvent(DEVICE_NAME_POT_1, potValue);
    lastValuePot1 = potValue;
  }
}

void reportButtonPress(int row, int col) {
  sendStringInputEvent(DEVICE_NAME_KEYPAD, String(keys[row][col]));
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
