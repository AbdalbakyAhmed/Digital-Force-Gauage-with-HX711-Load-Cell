#include <Keypad.h>
#include<LiquidCrystal.h>
#include<EEPROM.h>


//////////////////////////////////////////////////////////////////////////////

char password[4];
char pass[4], pass1[4];
char pass_position = 0;
#define pass_memo 0x0BB8 //EEPROM Address where PassCode stored "&3000"

void(* resetFunc) (void) = 0;
void setup()
{
  lcd.print(" Passcode Please:");
  lcd.setCursor(0, 1);
  for (char j=0;j<4;j++){pass[j]=EEPROM.read(pass_memo+j);}
}
void loop()
{
  customKey = customKeypad.getKey();
  if (customKey == '#')
    change();
  if (customKey == '*')
    resetFunc();
  if (customKey){
    password[pass_position++] = customKey;
    lcd.print(customKey);}
  if (pass_position == 4){ 
    pass_position = 0;
    delay(200);
  for (char j=0;j<4;j++){pass[j]=EEPROM.read(pass_memo+j);}
  if (!(strncmp(password, pass, 4))){
    pass_position = 0;
    digitalWrite(Relay, HiGH);
    lcd.clear();
    lcd.print("Passkey Accepted");
    lcd.setCursor(0, 1);
    lcd.print("#-Change");
    lcd.print("*-Lock");
    }
  else{
    digitalWrite(Relay, LOW);
    lcd.clear();
    lcd.print("Access Denied...");
    digitalWrite(buzzer , HiGH);
    delay(2000);
    lcd.clear();
    lcd.print("restarting...");
    delay(5000);
    pass_position = 0;
    resetFunc();}}
}

////////////////////////////////////////////////////////////////////////////////////

void change()
{
  char j = 0;
  lcd.clear();
  lcd.print("Current Passcode:");
  lcd.setCursor(0, 1);
  while (j < 4){
    char key = customKeypad.getKey();
    if (key){
      pass1[j++] = key;
      lcd.print(key);}
    key = 0;}
  delay(500);
  if ((strncmp(pass1, pass, 4))){
    lcd.clear();
    lcd.print("Wrong Passkey...");
    lcd.setCursor(1, 1);
    lcd.print("DENiED");
    digitalWrite(buzzer , HiGH);
    delay(2000);
    customKey = 0;
    resetFunc();
    }
  else{
    j = 0;
    lcd.clear();
    lcd.print("New Passcode:");
    lcd.setCursor(0, 1);
    while (j < 4){
      char key = customKeypad.getKey();
      if (key){
        pass[j] = key;
        lcd.print(key);
        EEPROM.write(pass_memo+j, key);
        j++;}}
    lcd.print("  Done......");
    delay(1000);
    resetFunc();}
}

