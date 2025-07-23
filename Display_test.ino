#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x3F try karo agar 0x27 pe kuch na aaye

void setup() {
  lcd.init();               // Use init() instead of begin()
  lcd.backlight();          // Turn on backlight
  lcd.setCursor(0, 0);
  lcd.print("LCD Working!");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("TechDelivers");
}

void loop() {}