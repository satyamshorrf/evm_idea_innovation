#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Fingerprint setup (use SoftwareSerial)
SoftwareSerial mySerial(2, 3); // RX, TX
Adafruit_Fingerprint finger(&mySerial);

// Button and LED pins
#define party1Btn A1
#define party2Btn A2
#define resultBtn A3
#define greenLED 12
#define whiteLED 13

int vote1 = 0;
int vote2 = 0;

void setup() {
  // Setup pins
  pinMode(party1Btn, INPUT_PULLUP);
  pinMode(party2Btn, INPUT_PULLUP);
  pinMode(resultBtn, INPUT_PULLUP);
  pinMode(greenLED, OUTPUT);
  pinMode(whiteLED, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Fingerprint sensor begin
  finger.begin(57600);
  if (finger.verifyPassword()) {
    lcd.setCursor(0,0);
    lcd.print("Fingerprint OK");
    delay(2000);
    lcd.clear();
  } else {
    lcd.setCursor(0,0);
    lcd.print("Sensor Error");
    while(1); // halt
  }

  // Welcome Screens
  lcd.setCursor(0, 0);
  lcd.print("Welcome Satyam");
  lcd.setCursor(0, 1);
  lcd.print("Continue No: 74");
  delay(3000);
  lcd.clear();
}

void loop() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place Finger...");

  int id = getFingerprintIDez(); // check fingerprint
  if (id != -1) { // valid fingerprint
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Access Granted");
    delay(1000);

    // Voting Display
    while(true) {
      lcd.setCursor(0, 0);
      lcd.print("BJP:");
      lcd.print(vote1);
      lcd.setCursor(8, 0);
      lcd.print("INC:");
      lcd.print(vote2);

      // Party 1 vote
      if (digitalRead(party1Btn) == LOW) {
        vote1++;
        digitalWrite(greenLED, HIGH);
        delay(300);
        digitalWrite(greenLED, LOW);
        while (digitalRead(party1Btn) == LOW); // Debounce
      }

      // Party 2 vote
      if (digitalRead(party2Btn) == LOW) {
        vote2++;
        digitalWrite(greenLED, HIGH);
        delay(300);
        digitalWrite(greenLED, LOW);
        while (digitalRead(party2Btn) == LOW); // Debounce
      }

      // Show result
      if (digitalRead(resultBtn) == LOW) {
        digitalWrite(whiteLED, HIGH);
        lcd.clear();

        if (vote1 == 0 && vote2 == 0) {
          lcd.setCursor(0, 0);
          lcd.print("No Votes Yet");
        } else if (vote1 > vote2) {
          lcd.setCursor(0, 0);
          lcd.print("BJP Wins!");
        } else if (vote2 > vote1) {
          lcd.setCursor(0, 0);
          lcd.print("INC Wins!");
        } else {
          lcd.setCursor(0, 0);
          lcd.print("Voting Tie!");
        }

        delay(4000);
        digitalWrite(whiteLED, LOW);
        vote1 = 0;
        vote2 = 0;
        lcd.clear();
      }
    }
  } else {
    lcd.setCursor(0,0);
    lcd.print("Access Denied");
    delay(2000);
    lcd.clear();
  }
}

// Fingerprint check function
int getFingerprintIDez() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;
  return finger.fingerID;
}
