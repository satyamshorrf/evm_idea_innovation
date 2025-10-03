#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// Fingerprint
#define FP_RX 2
#define FP_TX 3
SoftwareSerial fpSerial(FP_RX, FP_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fpSerial);

// LCD: RS=7,E=8,D4=9,D5=10,D6=11,D7=12
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Buttons
#define ENROLL_BTN 4
#define VOTE_A_BTN 5
#define VOTE_B_BTN 6

// LEDs & Buzzer
#define GREEN_LED 13
#define RED_LED A0
#define BUZZER A1

// EEPROM storage
#define ADDR_A 0
#define ADDR_B 4

void eepromWriteLong(int addr, long val) {
  for (int i = 0; i < 4; i++) EEPROM.update(addr + i, (val >> (8 * i)) & 0xFF);
}
long eepromReadLong(int addr) {
  long val = 0;
  for (int i = 0; i < 4; i++) val |= (long(EEPROM.read(addr + i)) << (8 * i));
  return val;
}

void setup() {
  Serial.begin(57600);
  fpSerial.begin(57600);
  finger.begin(57600);

  pinMode(ENROLL_BTN, INPUT_PULLUP);
  pinMode(VOTE_A_BTN, INPUT_PULLUP);
  pinMode(VOTE_B_BTN, INPUT_PULLUP);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);

  if (finger.verifyPassword()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FP Sensor OK");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("FP NOT Found!");
    while (1);
  }
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press ENROLL");
  lcd.setCursor(0, 1);
  lcd.print("or Scan Finger");
}

void buzzSuccess() {
  digitalWrite(GREEN_LED, HIGH);
  tone(BUZZER, 1000, 200);
  delay(200);
  digitalWrite(GREEN_LED, LOW);
}

void buzzError() {
  digitalWrite(RED_LED, HIGH);
  tone(BUZZER, 200, 500);
  delay(500);
  digitalWrite(RED_LED, LOW);
}

int enrollFingerprint(uint16_t id) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enrolling ID ");
  lcd.print(id);
  delay(1500);

  // 1st Image
  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(1) != FINGERPRINT_OK) return -1;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Remove finger");
  delay(1500);

  // 2nd Image
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place again...");
  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz(2) != FINGERPRINT_OK) return -2;

  if (finger.createModel() != FINGERPRINT_OK) return -3;
  if (finger.storeModel(id) != FINGERPRINT_OK) return -4;

  buzzSuccess();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enroll Success!");
  delay(2000);
  return 0;
}

void castVote(uint16_t fid) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome ID ");
  lcd.print(fid);
  lcd.setCursor(0, 1);
  lcd.print("Press A / B");

  while (true) {
    if (digitalRead(VOTE_A_BTN) == LOW) {
      long a = eepromReadLong(ADDR_A); eepromWriteLong(ADDR_A, a+1);
      lcd.clear(); lcd.print("Voted -> A");
      buzzSuccess();
      break;
    }
    if (digitalRead(VOTE_B_BTN) == LOW) {
      long b = eepromReadLong(ADDR_B); eepromWriteLong(ADDR_B, b+1);
      lcd.clear(); lcd.print("Voted -> B");
      buzzSuccess();
      break;
    }
  }
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan to Vote");
}

void loop() {
  // ENROLL Mode
  if (digitalRead(ENROLL_BTN) == LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ENROLL MODE");
    delay(1000);
    Serial.println("Enter ID in Serial Monitor:");
    while (Serial.available() == 0);
    int id = Serial.parseInt();
    if (id <= 0) id = 1; // default id
    if (enrollFingerprint(id) < 0) {
      buzzError();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enroll Failed");
    }
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready to Vote");
    return;
  }

  // VOTING Mode
  if (finger.getImage() == FINGERPRINT_OK) {
    if (finger.image2Tz() != FINGERPRINT_OK) { buzzError(); return; }
    if (finger.fingerFastSearch() != FINGERPRINT_OK) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("No Match!");
      buzzError();
      delay(1500);
      lcd.clear();
      lcd.print("Try Again...");
      return;
    } else {
      castVote(finger.fingerID);
    }
  }
}
