const int NUM_ROWS = 4;
const int NUM_COLS = 4;
const unsigned long DEBOUNCE_DELAY_MS = 50;

const String DEVICE_NAME_KEYPAD = "keypad";
const String INPUT_TYPE_STRING = "string";
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

void reportButtonPress(int row, int col) {
  Serial.print(START_BYTE);
  Serial.print("{\"device\": \"" + DEVICE_NAME_KEYPAD + "\", \"inputType\": \"" + INPUT_TYPE_STRING + "\", \"value\": \"" + keys[row][col] + "\"}");
  Serial.print(END_BYTE);
}
