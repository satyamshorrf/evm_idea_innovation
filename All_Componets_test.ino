#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// Fingerprint sensor setup
SoftwareSerial fingerSerial(2, 3); // RX=D2 (white), TX=D3 (green)
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Try 0x3F if LCD fails

// Pin definitions
const int whiteLED = 12;
const int greenLED = 13;
const int buzzerPin = A0;
const int buttonPins[] = {A1, 7, 8, 9}; // Add, Delete, Result, Party
const int touchSensePin = 4; // Blue wire for touch sense (optional)

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize pins
  pinMode(whiteLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(touchSensePin, INPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Test LEDs and buzzer
  digitalWrite(whiteLED, HIGH);
  digitalWrite(greenLED, HIGH);
  tone(buzzerPin, 1000, 500);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("LEDs & Buzzer ON");
  Serial.println("Testing LEDs and Buzzer...");
  delay(2000);
  digitalWrite(whiteLED, LOW);
  digitalWrite(greenLED, LOW);
  lcd.clear();
  lcd.print("LED Test Done");
  delay(1000);

  // Initialize fingerprint sensor
  fingerSerial.begin(57600); // Try 9600, 19200, or 115200 if needed
  lcd.clear();
  lcd.print("Testing Sensor...");
  Serial.println("Testing Fingerprint Sensor...");
  if (finger.verifyPassword()) {
    lcd.clear();
    lcd.print("Sensor OK");
    Serial.println("Fingerprint sensor found!");
    Serial.print("Templates: ");
    Serial.println(finger.getTemplateCount());
  } else {
    lcd.clear();
    lcd.print("Sensor Error!");
    Serial.println("Fingerprint sensor not found!");
    delay(2000);
  }
  lcd.clear();
  lcd.print("Place Finger");
  digitalWrite(whiteLED, HIGH);
}

void loop() {
  // Test touch sense
  if (digitalRead(touchSensePin) == HIGH) {
    Serial.println("Finger detected on touch sense!");
    lcd.clear();
    lcd.print("Finger Detected");
    delay(1000);
    lcd.clear();
    lcd.print("Place Finger");
  }

  // Test buttons
  for (int i = 0; i < 4; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Button ");
      lcd.print(i + 1);
      lcd.print(" Pressed");
      Serial.print("Button ");
      Serial.print(i + 1);
      Serial.println(" pressed");
      tone(buzzerPin, 1500, 200);
      digitalWrite(greenLED, HIGH);
      delay(200);
      digitalWrite(greenLED, LOW);
      while (digitalRead(buttonPins[i]) == LOW);
      lcd.clear();
      lcd.print("Place Finger");
    }
  }

  // Test fingerprint sensor
  fingerSerial.listen();
  int fingerprintID = getFingerprintID();
  if (fingerprintID > 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ID: ");
    lcd.print(fingerprintID);
    Serial.print("Fingerprint ID: ");
    Serial.println(fingerprintID);
    digitalWrite(whiteLED, LOW);
    digitalWrite(greenLED, HIGH);
    tone(buzzerPin, 2000, 200);
    delay(2000);
    digitalWrite(greenLED, LOW);
    digitalWrite(whiteLED, HIGH);
    lcd.clear();
    lcd.print("Place Finger");
  } else if (fingerprintID == -2) {
    lcd.clear();
    lcd.print("No Finger!");
    Serial.println("No finger detected");
    delay(1000);
    lcd.clear();
    lcd.print("Place Finger");
  } else if (fingerprintID == -1) {
    lcd.clear();
    lcd.print("Scan Error!");
    Serial.println("Fingerprint scan error");
    delay(1000);
    lcd.clear();
    lcd.print("Place Finger");
  }
}

int getFingerprintID() {
  uint8_t p = finger.getImage();
  Serial.print("getImage: ");
  Serial.println(p);
  if (p == FINGERPRINT_NOFINGER) return -2;
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  Serial.print("image2Tz: ");
  Serial.println(p);
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  Serial.print("fingerFastSearch: ");
  Serial.println(p);
  if (p == FINGERPRINT_OK) return finger.fingerID;
  return -1;
}