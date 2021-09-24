#include <Keypad.h>
#include <LiquidCrystal.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'},
  {'*','D','#','F'}
};
byte rowPins[ROWS] = {26,27,28,29}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {22,23,24,25}; //connect to the column pinouts of the keypad
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
int v1 = 0;
int v2 = 0;
int v3 = 0;
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
int firtvariable;
void setup(){
  Serial.begin(9600);
  lcd.begin(20,4);
}


void loop()
{
   v1 = GetNumber();
   Serial.println ();
   Serial.print (v1);
   v2 = GetNumber();
   v3 = GetNumber();

}
int GetNumber()
{
   int num = 0;
   char key = customKeypad.getKey();
   while(key != '#')
   {
      switch (key)
      {
         case NO_KEY:
            break;

         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
            lcd.print(key);
            num = num * 10 + (key - '0');
            break;

         case '*':
            num = 0;
            lcd.clear();
            break;
      }

      key = customKeypad.getKey();
   }

   return num;
}
