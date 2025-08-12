#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize 20x4 LCD at I2C address 0x27 (try 0x3F if not working)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Button and LED pins
#define party1Btn A1
#define party2Btn A2
#define resultBtn A3
#define greenLED 12
#define whiteLED 13

int vote1 = 0;
int vote2 = 0;

void setup() {
  // Setup buttons as input with pull-up
  pinMode(party1Btn, INPUT_PULLUP);
  pinMode(party2Btn, INPUT_PULLUP);
  pinMode(resultBtn, INPUT_PULLUP);

  // Setup LEDs as output
  pinMode(greenLED, OUTPUT);
  pinMode(whiteLED, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Welcome screens
  lcd.setCursor(2, 0);
  lcd.print("17 BIHAR BN NCC");
  lcd.setCursor(3, 1);
  lcd.print("CADET SATYAM");
  delay(3000);
  lcd.clear();

  lcd.setCursor(2, 0);
  lcd.print("Voting Machine");
  lcd.setCursor(2, 1);
  lcd.print("TPC Madhepura");
  delay(3000);
  lcd.clear();

  lcd.setCursor(2, 0);
  lcd.print("Welcome Satyam");
  lcd.setCursor(2, 1);
  lcd.print("Continue No: 74");
  delay(3000);
  lcd.clear();
}

void loop() {
  // Show current vote count
  lcd.setCursor(0, 0);
  lcd.print("Party BJP Votes: ");
  lcd.print(vote1);
  lcd.setCursor(0, 1);
  lcd.print("Party INC Votes: ");
  lcd.print(vote2);
  lcd.setCursor(0, 2);
  lcd.print("Press Btn to Vote ");
  lcd.setCursor(0, 3);
  lcd.print("Show Result: A3 Btn");

  // Vote for BJP
  if (digitalRead(party1Btn) == LOW) {
    vote1++;
    digitalWrite(greenLED, HIGH);
    delay(300);
    digitalWrite(greenLED, LOW);
    while (digitalRead(party1Btn) == LOW); // Wait until button is released
    delay(300); // Debounce
  }

  // Vote for INC
  if (digitalRead(party2Btn) == LOW) {
    vote2++;
    digitalWrite(greenLED, HIGH);
    delay(300);
    digitalWrite(greenLED, LOW);
    while (digitalRead(party2Btn) == LOW); // Wait until button is released
    delay(300); // Debounce
  }

  // Show result
  if (digitalRead(resultBtn) == LOW) {
    digitalWrite(whiteLED, HIGH);
    lcd.clear();

    if (vote1 == 0 && vote2 == 0) {
      lcd.setCursor(3, 1);
      lcd.print("No Votes Yet");
    } else if (vote1 > vote2) {
      lcd.setCursor(3, 1);
      lcd.print("BJP Wins!");
    } else if (vote2 > vote1) {
      lcd.setCursor(3, 1);
      lcd.print("INC Wins!");
    } else {
      lcd.setCursor(3, 1);
      lcd.print("Voting Tie!");
    }

    delay(4000);
    digitalWrite(whiteLED, LOW);
    vote1 = 0;
    vote2 = 0;
    lcd.clear();
  }
}
