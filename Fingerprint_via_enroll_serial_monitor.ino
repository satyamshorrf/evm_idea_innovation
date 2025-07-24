#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

SoftwareSerial fingerSerial(2, 3);  // RX, TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);

uint8_t id = 1;  // Change for multiple fingerprints

void setup() {
  Serial.begin(9600);
  while (!Serial);
  delay(1000);
  fingerSerial.begin(57600); // Try 9600 if this fails

  if (finger.verifyPassword()) {
    Serial.println("✅ Fingerprint sensor detected");
  } else {
    Serial.println("❌ Sensor not found");
    while (1);
  }

  Serial.println("Ready to enroll a fingerprint!");
  Serial.println("Type 'e' to enroll...");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'e') {
      enrollFinger(id);
      id++;
    }
  }
}

uint8_t enrollFinger(uint8_t id) {
  int p = -1;
  Serial.print("Enrolling ID #"); Serial.println(id);

  while (p != FINGERPRINT_OK) {
    Serial.println("Place finger...");
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) continue;
    if (p != FINGERPRINT_OK) return p;
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) return p;

  Serial.println("Remove finger");
  delay(2000);
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  Serial.println("Place same finger again");
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) continue;
    if (p != FINGERPRINT_OK) return p;
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) return p;

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("❌ Fingerprint doesn't match");
    return p;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("✅ Fingerprint enrolled successfully!");
  } else {
    Serial.println("❌ Failed to store fingerprint");
  }
  return p;
}
