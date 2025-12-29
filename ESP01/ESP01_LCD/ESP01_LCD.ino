/*********
  Project:

  ESP01 with I2C 16x2 character LCD Display Using PCF8574AT I2C Module with Buzzer

  Power Supply:
  
  For ESP01 use 3.3V Power Supply
  For I2C LCD use 5V Power Supply
  Buzzer can be powered from 3.3V or 5V

  Wiring:
  ESP01    I2C LCD
   GPIO0  -> SDA
   GPIO2  -> SCL
   GPIO3  -> Buzzer Signal

  Library Used:
  
  LiquidCrystal_I2C (See: ESP01/Required Libraries/LiquidCrystal_I2C)
  Wire Library

*********/

#include <LiquidCrystal_I2C.h>
#include <Wire.h>


//  set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

const int buzzerPin = 3;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

void setup()
{
  Wire.begin(0, 2);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}

void loop()
{

  for (int i = 0; i < lcdRows; i++)
  {
    // set cursor to first column, first row
    lcd.setCursor(i, i);
    // print message
    lcd.print("Om Namah Shivay");
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    delay(800);
    // clears the display to print new message
    lcd.clear();
  }
  // set cursor to first column, second row
}