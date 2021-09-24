/* @file CustomKeypad.pde
|| @version 1.0
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Demonstrates changing the keypad size and key values.
|| #
*/
#include <Keypad.h>
#include <LiquidCrystal.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'},
  {'C','D','E','F'}
};
byte rowPins[ROWS] = {26,27,28,29}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {22,23,24,25}; //connect to the column pinouts of the keypad
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
int firtvariable;
void setup(){
  Serial.begin(9600);
  lcd.begin(20,4);
}
  
void loop(){
  
  // for 12V -----------------------------------------------------------
 char key1 = customKeypad.getKey();
 switch (key1) {
   case NO_KEY:
   break;

   case '0': case '1': case '2': case '3': case '4': 
   case '5': case '6': case '7': case '8': case '9': 
   lcd.print(key1);
       firstvariable = firstvariable* 10 + key1 - '0'; 
   break;

   case '#': 
       Serial.println ();
   Serial.print ("Value for 12V is: ");
   Serial.print (firstvariable );
       break;

   case '*': 
       firstvariable = 0;
       lcd.clear();
       Serial.println ();
   Serial.print ("Reset value:");
   Serial.print (firstvariable );
 }
 // for 12V end -----------------------------------------------------------

}
