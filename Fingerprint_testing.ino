#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Fingerprint sensor
SoftwareSerial fingerSerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

// Pins
#define addBtn A1
#define delBtn 7
#define resultBtn 8
#define voteBtn 9
#define upBtn 5
#define downBtn 6
#define greenLED 13
#define whiteLED 12
#define buzzerPin A0

// Global
int selectedParty = 0;
String partyList[2] = {"Party A", "Party B"};
int voteCount[2] = {0, 0};

// Setup
void setup() {
  Serial.begin(9600);
  fingerSerial.begin(57600);

  lcd.init();
  lcd.backlight();
  pinMode(addBtn, INPUT_PULLUP);
  pinMode(delBtn, INPUT_PULLUP);
  pinMode(resultBtn, INPUT_PULLUP);
  pinMode(voteBtn, INPUT_PULLUP);
  pinMode(upBtn, INPUT_PULLUP);
  pinMode(downBtn, INPUT_PULLUP);
  pinMode(greenLED, OUTPUT);
  pinMode(whiteLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("EVM Ready...");
  delay(2000);
  lcd.clear();

  if (!finger.verifyPassword()) {
    lcd.print("Sensor Error!");
    while (1);
  }

  lcd.print("Place Finger...");
  digitalWrite(whiteLED, HIGH);
}

// Main loop
void loop() {
  if (digitalRead(addBtn) == LOW) {
    addFingerprint();
  }

  if (digitalRead(delBtn) == LOW) {
    deleteFingerprint();
  }

  if (digitalRead(resultBtn) == LOW) {
    showResults();
  }

  int id = getFingerprintID();
  if (id > 0) {
    lcd.clear();
    lcd.print("User ID: ");
    lcd.print(id);
    delay(1500);
    voteMenu(id);
  }
}

// Enroll Finger
void addFingerprint() {
  lcd.clear();
  lcd.print("Enroll Finger...");
  delay(1000);
  for (int id = 1; id < 20; id++) {
    if (finger.loadModel(id) != FINGERPRINT_OK) {
      enrollFinger(id);
      return;
    }
  }
  lcd.clear();
  lcd.print("Limit Reached!");
  delay(1000);
}

// Delete Finger
void deleteFingerprint() {
  lcd.clear();
  lcd.print("Del ID 1 to 20...");
  delay(1000);
  for (int id = 1; id < 20; id++) {
    if (finger.deleteModel(id) == FINGERPRINT_OK) {
      lcd.clear();
      lcd.print("Deleted ID: ");
      lcd.print(id);
      delay(1000);
    }
  }
}

// Voting Menu
void voteMenu(int userID) {
  selectedParty = 0;
  while (true) {
    lcd.clear();
    lcd.print("Vote: ");
    lcd.print(partyList[selectedParty]);
    delay(200);

    if (digitalRead(upBtn) == LOW) {
      selectedParty = (selectedParty + 1) % 2;
      delay(300);
    }

    if (digitalRead(downBtn) == LOW) {
      selectedParty = (selectedParty - 1 + 2) % 2;
      delay(300);
    }

    if (digitalRead(voteBtn) == LOW) {
      voteCount[selectedParty]++;
      lcd.clear();
      lcd.print("Voted: ");
      lcd.print(partyList[selectedParty]);
      digitalWrite(greenLED, HIGH);
      tone(buzzerPin, 2000, 200);
      delay(1500);
      digitalWrite(greenLED, LOW);
      lcd.clear();
      lcd.print("Place Finger...");
      return;
    }
  }
}

// Show Result
void showResults() {
  lcd.clear();
  lcd.print("A:");
  lcd.print(voteCount[0]);
  lcd.setCursor(8, 0);
  lcd.print("B:");
  lcd.print(voteCount[1]);
  delay(4000);
  lcd.clear();
  lcd.print("Place Finger...");
}

// Finger Matching
int getFingerprintID() {
  finger.getImage();
  if (finger.image2Tz() != FINGERPRINT_OK) return -1;
  if (finger.fingerFastSearch() != FINGERPRINT_OK) return -1;
  return finger.fingerID;
}

// Enroll Logic
uint8_t enrollFinger(uint8_t id) {
  int p = -1;
  lcd.clear();
  lcd.print("Place Finger...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }
  if (finger.image2Tz(1) != FINGERPRINT_OK) return p;
  lcd.clear();
  lcd.print("Remove...");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER);
  lcd.clear();
  lcd.print("Again...");
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }
  if (finger.image2Tz(2) != FINGERPRINT_OK) return p;
  if (finger.createModel() != FINGERPRINT_OK) return p;
  if (finger.storeModel(id) == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Enrolled ID:");
    lcd.print(id);
    delay(1000);
  }
  return p;
}