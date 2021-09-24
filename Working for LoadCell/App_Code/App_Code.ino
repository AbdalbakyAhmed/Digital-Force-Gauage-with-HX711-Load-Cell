/* This code will initialize the Keypad$LCD 
 *  ,create the App (scroll up&down)
 *  ,passcode 
 */

#include <Keypad.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// for Keypad Implementation
#define ROWS 4 //four rows
#define COLS 4 //four columns

//define the cymbols on the buttons of the keypads
char Keys[ROWS][COLS] = {
  {'1','2','3','U'},
  {'4','5','6','D'},
  {'7','8','9','E'},
  {'.','0','P','B'}
};
/////////////////////
char password[4];
char pass[4], pass1[4];
char pass_position = 0;
#define pass_memo 0x0BB8 //EEPROM Address where PassCode stored "&3000"
char customKey=0,pass_count=0;
boolean access=false;
void(* resetFunc) (void) = 0;
////////////////////
// for connection of Keypad
byte rowPins[ROWS] = {26,27,28,29}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {22,23,24,25}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(Keys), rowPins, colPins, ROWS, COLS); 

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
/////////////////
char Key,Key2,Key3,System_Key,CalibChoice;
void setup(){
  lcd.begin(20,4);
  //digitalClockDisplay();//Show the data of Day on the screen 
  ///////////
  for (char j=0;j<4;j++){pass[j]=EEPROM.read(j);}
  //////////
}
void loop(){
  byte st=0; //counter for steps on App scrolling
  Key = customKeypad.getKey();
  //if we press print firstly
  st=0;
  while(Key=='E'){
    //lcd.clear();
    lcd.print(" Settings");
    lcd.setCursor(0,1);
    lcd.print(" Calibration Report");
    lcd.setCursor(0,3);
    lcd.print(" Or press print");
    //lcd.setCursor(0,0);
    //lcd.blink();
    Key2=customKeypad.getKey();
    if(Key2=='D'){
      st++;}
    else if(Key2=='U'){st=0;}
    else{}
    if(st==0){
      lcd.setCursor(0,0);
      lcd.blink();
      delay(50);}
    else if(st>=1){
      lcd.setCursor(0,1);
      lcd.blink();
      delay(50);}
    else{}  
    Key2=customKeypad.getKey();
    byte l=0;  
    while(Key2=='E'&&st==0){
      /* Password system */
      if(l==0){lcd.clear();delay(200);l++;}
      lcd.setCursor(0,0);lcd.print("Passcode Please:");
      customKey=customKeypad.getKey();
      if (customKey=='0'||customKey=='2'||customKey=='1'||customKey=='3'||customKey=='4'
      ||customKey=='5'||customKey=='6'||customKey=='7'||customKey=='8'||customKey=='9'){ 
        password[pass_count++] = customKey;
        lcd.setCursor(pass_count-1,1);lcd.print(customKey);}
      if (pass_count == 4){ 
        pass_count = 0;
        delay(200);
        for (char j=0;j<4;j++){pass[j]=EEPROM.read(j);}
        char p=0;
        if (!(strncmp(password, pass, 4))){
          pass_count = 0;
          while(!(strncmp(password, pass, 4))){
          if(p==0){lcd.clear();lcd.print("Passkey Accepted!");delay(2000);lcd.clear();p++;}
          lcd.setCursor(0,0);lcd.print("Press: 1.change pass");
          lcd.setCursor(0,1);lcd.print("2.OverFlow value");
          lcd.setCursor(0,2);lcd.print("3.TouchValue value");
          System_Key=customKeypad.getKey();
          if(System_Key=='1'){change();}
          
          }}
        else{
          lcd.clear();
          lcd.print("Access Denied...");delay(2000);
          pass_count = 0;//resetFunc();
          }}
      /* Password system */
      //Key2=customKeypad.getKey();if(Key2!='B'){Key2='E';st=0;}
      if(customKey=='B') {Key2='B';Key='E';lcd.clear();st=0;}
    }    
    while(Key2=='E'&&st>=1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Calibration Menu");
      lcd.setCursor(0,1);
      lcd.print("1-5  2-10  3-15Kg");
      //CalibChoice=customKeypad.getKey();
      /* Calibration System */
      Key2=customKeypad.getKey();
      if(Key2!='B'){Key2='E';st=1;}
      else {Key2='B';Key='E';lcd.clear();st=0;}
    }
    Key=customKeypad.getKey();
    if(Key!='B'){Key='E';}
  }  
}


////////////////////////////////


void change(){
  int j = 0;
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
    lcd.print("DENIED");
    delay(2000);lcd.clear();
    System_Key = 0;}
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
        EEPROM.write(j, key);
        j++;}}
    lcd.print("  Done......");
    delay(2000);lcd.clear();}
}
