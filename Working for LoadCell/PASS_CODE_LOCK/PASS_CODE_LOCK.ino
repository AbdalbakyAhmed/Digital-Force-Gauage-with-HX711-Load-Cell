/*
    +------------------------------------------------------------------------+
    |                                                                        |
    |               DUINO LOCK SYTSTEM FIRM_WARE v1.1.0                      |
    |                       BY_:DIY MECHANICS                                |
    |    --------------------------------------------------------------      |
    |  written_by:suhail  project:HOME AUTOMATION:-passcode lock sytem v1.1  |
    |  facebook.com/suhail jr.                      twitter.com/suhail jr.   |
    |             visit more @ www.diymechanics.wordpress.com                |
    +------------------------------------------------------------------------+
*/

///////////////////////////////////////////////////////////////////////////////


#include <Keypad.h>
#include<LiquidCrystal.h>
#include<EEPROM.h>


//////////////////////////////////////////////////////////////////////////////


LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
char password[4];
char pass[4], pass1[4];
int i = 0;
int buzzer = A0;
int Relay = A5;
int staterelay = LOW;
char customKey = 0;


//////////////////////////////////////////////////////////////////////////////


const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};


///////////////////////////////////////////////////////////////////////////////



byte rowPins[ROWS] = {7, 6, 5, 4}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {3, 2, 1, 0}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);



////////////////////////////////////////////////////////////////////////////////////

void(* resetFunc) (void) = 0;
void setup()
{
  pinMode(Relay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(Relay, LOW);
  digitalWrite(buzzer , LOW);
  lcd.begin(16, 2);
  lcd.print("STARK-TECHNOLOGY");
  lcd.setCursor(0, 1);
  delay(4000);
  lcd.print("BOOTING DEVICE");
  delay(1000);
  digitalWrite(buzzer , HIGH);
  delay(100);
  digitalWrite(buzzer , LOW);
  delay(50);
  digitalWrite(buzzer , HIGH);
  delay(200);
  digitalWrite(buzzer , LOW);
  delay(50);
  digitalWrite(buzzer , HIGH);
  delay(200);
  digitalWrite(buzzer , LOW);
  delay(100);
  digitalWrite(buzzer , LOW);
  lcd.clear();
  lcd.print("KEYPAD-LOCKED");
  lcd.clear();
  lcd.print(" Passcode Please:");
  lcd.setCursor(0, 1);
  for (int j = 0; j < 4; j++)
    pass[j] = EEPROM.read(j);

  ////////////////////////////////////////////////////////////////////////////////////

}
void loop()
{
  customKey = customKeypad.getKey();
  if (customKey == '#')
    change();

  if (customKey == '*')
    resetFunc();


  if (customKey)

  {
    password[i++] = customKey;
    lcd.print(customKey);

  }

  if (i == 4)
  { i = 0;
    delay(200);
    for (int j = 0; j < 4; j++)
      pass[j] = EEPROM.read(j);
    if (!(strncmp(password, pass, 4)))
    {
      i = 0;
      digitalWrite(Relay, HIGH);
      lcd.clear();
      lcd.print("Passkey Accepted");
      digitalWrite(buzzer , HIGH);
       delay(100);
       digitalWrite(buzzer , LOW);
  delay(50);
  digitalWrite(buzzer , HIGH);
  delay(90);
  digitalWrite(buzzer , LOW);
  delay(30);
  digitalWrite(buzzer , HIGH);
  delay(200);
  digitalWrite(buzzer , LOW);
  delay(100);
  digitalWrite(buzzer , HIGH);
  delay(200);
  digitalWrite(buzzer , LOW);
  delay(50);
  digitalWrite(buzzer , HIGH);
  delay(500);digitalWrite(buzzer , LOW);
  delay(100);
      lcd.setCursor(0, 1);
      lcd.print("#-Change");
      lcd.print("*-Lock");





    }

    else
    {
      digitalWrite(Relay, LOW);
      
      lcd.clear();
      lcd.print("Access Denied...");
       digitalWrite(buzzer , HIGH);
      delay(2000);
      
      lcd.clear();
      lcd.print("restarting...");
      delay(5000);
      i = 0;
      resetFunc();


    }

  }
}

////////////////////////////////////////////////////////////////////////////////////

void change()
{
  int j = 0;
  lcd.clear();
  lcd.print("Current Passcode:");
  lcd.setCursor(0, 1);
  while (j < 4)
  {
    char key = customKeypad.getKey();
    if (key)
    {
      pass1[j++] = key;
      lcd.print(key);

    }
    key = 0;
  }
  delay(500);

  if ((strncmp(pass1, pass, 4)))
  {
    lcd.clear();
    lcd.print("Wrong Passkey...");

    lcd.setCursor(1, 1);
    lcd.print("DENIED");
    digitalWrite(buzzer , HIGH);
    delay(2000);
    customKey = 0;
    resetFunc();
  }
  else
  {
    j = 0;

    lcd.clear();
    lcd.print("New Passcode:");
    lcd.setCursor(0, 1);
    while (j < 4)
    {
      char key = customKeypad.getKey();
      if (key)

      {
        pass[j] = key;
        lcd.print(key);
        EEPROM.write(j, key);
        j++;

      }
    }
    lcd.print("  Done......");
    delay(1000);
    digitalWrite(Relay, LOW);
    resetFunc();
  }


}
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
/*===================================================================================
   recover code: HAp-1.0.0
*/





