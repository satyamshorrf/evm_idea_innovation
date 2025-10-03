#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Fingerprint sensor on pin 2 & 3
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger(&mySerial);

// Pins
#define party1Btn A1
#define party2Btn A2
#define resultBtn A3
#define greenLED 12
#define buzzer 11

int vote1 = 0, vote2 = 0;
bool votingEnabled = false;  // voting allowed only after enroll
bool hasVoted = false;       // one vote per person

void setup() {
  pinMode(party1Btn, INPUT_PULLUP);
  pinMode(party2Btn, INPUT_PULLUP);
  pinMode(resultBtn, INPUT_PULLUP);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.init();
  lcd.backlight();

  finger.begin(57600);
  if (!finger.verifyPassword()) {
    lcd.print("Sensor ERROR!");
    while (1);
  }

  lcd.setCursor(0, 0);
  lcd.print("Voting Machine");
  delay(2000);
  lcd.clear();

  // Fingerprint Enroll step
  enrollFingerprint(1); // Save at ID=1
  votingEnabled = true;

  lcd.clear();
  lcd.print("Voting Enabled");
  delay(2000);
  lcd.clear();
}

void loop() {
  if (!votingEnabled) return;  // block until enrollment done
  if (hasVoted) {
    lcd.setCursor(0,0);
    lcd.print("Vote Cast Done");
    return;
  }

  // Voting Screen
  lcd.setCursor(0, 0);
  lcd.print("Press Button...");
  lcd.setCursor(0, 1);
  lcd.print("A1=BJP  A2=INC");

  // BJP vote
  if (digitalRead(party1Btn) == LOW) {
    vote1++;
    confirmVote("BJP");
    hasVoted = true;
  }

  // INC vote
  if (digitalRead(party2Btn) == LOW) {
    vote2++;
    confirmVote("INC");
    hasVoted = true;
  }

  // Show Result Button
  if (digitalRead(resultBtn) == LOW) {
    showResult();
  }
}

// ================= FUNCTIONS =================

void confirmVote(String party) {
  lcd.clear();
  lcd.print("Vote -> " + party);
  digitalWrite(greenLED, HIGH);
  tone(buzzer, 1000, 300);
  delay(500);
  digitalWrite(greenLED, LOW);
  noTone(buzzer);
  lcd.clear();
  lcd.print("Vote Success!");
  delay(2000);
  lcd.clear();
}

void showResult() {
  lcd.clear();
  if (vote1 == 0 && vote2 == 0) lcd.print("No Votes Yet");
  else if (vote1 > vote2) lcd.print("BJP Wins!");
  else if (vote2 > vote1) lcd.print("INC Wins!");
  else lcd.print("Tie!");

  delay(4000);
  lcd.clear();
}

// Enroll fingerprint only once
void enrollFingerprint(uint8_t id) {
  int p = -1;
  lcd.clear();
  lcd.print("Enroll Finger");
  delay(1000);

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }
  finger.image2Tz(1);

  lcd.clear();
  lcd.print("Remove Finger");
  delay(2000);

  while (finger.getImage() != FINGERPRINT_NOFINGER);

  lcd.clear();
  lcd.print("Place Again");
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }
  finger.image2Tz(2);

  if (finger.createModel() == FINGERPRINT_OK) {
    if (finger.storeModel(id) == FINGERPRINT_OK) {
      lcd.clear();
      lcd.print("Enroll Done!");
      tone(buzzer, 800, 200);
      digitalWrite(greenLED, HIGH);
      delay(500);
      digitalWrite(greenLED, LOW);
      noTone(buzzer);
      delay(2000);
    }
  }
}
