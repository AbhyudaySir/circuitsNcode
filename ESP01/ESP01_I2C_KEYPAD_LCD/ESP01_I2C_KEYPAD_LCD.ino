/*********
  Project:

  ESP01 with I2C 16x2 character LCD Display Using PCF8574AT I2C Module with Buzzer and I2C 4x4 Keypad Using PCF8574 I2C Module

  Power Supply:
  
  For ESP01 use 3.3V Power Supply
  For I2C Daisy Chain (LCD and Keypad I2C Backpack) use 5V Power Supply
  Buzzer can be powered from 3.3V or 5V

  Wiring:
  ESP01    I2C PCF8574T Keypad  I2C PCF8574AT LCD
   GPIO0        -> SDA             -> SDA
   GPIO2        -> SCL             -> SCL

   Also connect Buzzer as below 

   GPIO3  -> Buzzer Signal (Use 1K resistor in series)

   IO Pins of PCF8574T Keypad Module
    P0...7 -> Columns and Rows of Keypad

  Library Used:
  
  LiquidCrystal_I2C (See: ESP01/Required Libraries/LiquidCrystal_I2C)
  Wire Library
  Keypad Library (See: ESP01/Required Libraries/Keypad)
  Keypad_I2C Library (See: ESP01/Required Libraries/Keypad_I2C)

*********/ 

#include <Wire.h>
#include <Keypad.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>

//pcf8574AT - LCD I2C backpack

//pcf8574T - keypad I2C backpack (I/O expander)

//0 to sda
//2 to scl

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {0, 1, 2, 3}; //connect to the column pinouts of the keypad

// PCF8574 I2C address (common addresses are 0x20 or 0x27)
#define I2CADDR 0x20

const int buzzerPin = 3;

//set Buzzer Time
const int inputBeep = 100;
const int actionBeep = 300;


// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

// Initialize the I2CKeyPad library
Keypad_I2C customKeypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS, I2CADDR); 


void beep(int beepType){
  digitalWrite(buzzerPin, HIGH);
  delay(beepType);
  digitalWrite(buzzerPin, LOW);
}

void setup(){

  Wire.begin(0, 2);
  // initialize LCD
  lcd.init();
  customKeypad.begin();

  // turn on LCD backlight                      
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Press The Key");

}

void loop(){
  
    // read the keypad key press
  char customKey = customKeypad.getKey();

    // if the key is pressed
    if (customKey){

      // clear the display to print new message  
      lcd.setCursor(0, 0);
      beep(inputBeep);
      lcd.print("Key Pressed: ");
      lcd.setCursor(1, 1);
      // print the key pressed
      lcd.print(customKey);
      delay(3000);
      beep(actionBeep);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Press The Key");
    }
  }