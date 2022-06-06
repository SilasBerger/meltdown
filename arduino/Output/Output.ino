#include <LiquidCrystal.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// === LED settings ===
const int NUM_LEDS = 4;

// === OLED settings ===
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 32;
const byte SCREEN_ADDRESS = 0x3C;
const int OLED_RESET = -1;

// === Pinout ===
const byte lcdRs = 12;
const byte lcdEn = 11;
const byte lcdD4 = 5;
const byte lcdD5 = 4;
const byte lcdD6 = 3;
const byte lcdD7 = 2;
const byte ledPins[NUM_LEDS] = {A0, A1, A2, A3};
const byte buzzerPin = 13;

LiquidCrystal lcd(lcdRs, lcdEn, lcdD4, lcdD5, lcdD6, lcdD7);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  initLcd();
  initLeds();
  initOled();
}

void initLcd() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Ready...");
  
}

void initLeds() {
  for (int index = 0; index < NUM_LEDS; index++) {
    pinMode(ledPins[index], OUTPUT);
  }
}

void initOled() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
}

void loop() {
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    lcd.clear();
    lcd.print("Button " + String(incomingByte + 1));
    for (int index = 0; index < NUM_LEDS; index++) {
      digitalWrite(ledPins[index], LOW);
    }
    digitalWrite(ledPins[incomingByte], HIGH);

    for (int i = 0; i <= incomingByte; i++) {
      tone(buzzerPin, 261, 100);
      delay(200);
    }
  }
}
