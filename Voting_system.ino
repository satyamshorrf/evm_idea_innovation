#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup (use 0x3F if 0x27 doesn't work)
LiquidCrystal_I2C lcd(0x27, 16, 2);

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

  // Welcome screen
  lcd.setCursor(0, 0);
  lcd.print("17 BIHAR BN NCC");
  lcd.setCursor(0, 1);
  lcd.print("CADET SATYAM");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Voting Machine");
  lcd.setCursor(0, 1);
  lcd.print("TPC Madhepura");
  delay(3000);
  lcd.clear();
}

void loop() {
  // Display ongoing vote count
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
    delay(300);
  }

  // Party 2 vote
  if (digitalRead(party2Btn) == LOW) {
    vote2++;
    digitalWrite(greenLED, HIGH);
    delay(300);
    digitalWrite(greenLED, LOW);
    while (digitalRead(party2Btn) == LOW);
    delay(300);
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