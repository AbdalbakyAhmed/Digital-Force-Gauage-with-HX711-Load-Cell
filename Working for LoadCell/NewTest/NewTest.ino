#include <Keypad.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <avr/eeprom.h>
#include "RealTime.h"
#include "Types.h"
#include "HW_Interface.h"
#include "QuickStats.h"



//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(Keys), rowPins, colPins, ROWS, COLS);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
float value;
/*char memo_step=0;
  char last_memo;
  typedef struct {
  float load;
  int Hr;
  int Min;
  int Sec;
  }create;
  typedef struct{float load;
  int Hr;
  int Min;
  int Sec;
  }infostored;
  int structSize=sizeof(create);
  create sample[100];*/
float load;
int Hr;
int Min;
int Sec;
double N = 0;
QuickStats stats;
void setup() {
  Serial.begin(9600);
  Time_set(); //Sync. for settint correct Time
  //digitalClockDisplay();//Show the data of Day on LCD
  lcd.begin(20, 4);
  //Serial.println("Plz Enter value to store or 1 to printout");

}
void loop() {
  if (Serial.available() > 0) {
    value = Serial.read();
    if (value > 70) {
      EEPROM.put(N, value);
      EEPROM.write(N + 4, hour());
      EEPROM.write(N + 6, minute());
      EEPROM.write(N + 8, second());
      N = N + 10;
      EEPROM.update(3000, N / 10);
      /*sample[memo_step].load=value;
        sample[memo_step].Hr=hour();
        sample[memo_step].Min=minute();
        sample[memo_step].Sec=second();
        eeprom_write_block((const void*)&sample,(void*)(memo_step*structSize),structSize);
        memo_step++;
        last_memo=memo_step;
        EEPROM.update(300,last_memo);*/
    }
    else if (value == '1') {
      int x = EEPROM.read(3000);
      float d[x]; int cou = 0; float data;
      for (int z = 0; z <=x; z++) {
        EEPROM.get(cou, data); d[z] = data;
        cou = cou + 10;
      }
      Serial.print("Manufacturer : "); Serial.print("\n");
      Serial.print("Customer : "); Serial.print("\n");
      Serial.print("Batch ID : "); Serial.print("\n");
      Serial.print("Date : ");Serial.print(day()); Serial.print("/"); Serial.print(month()); Serial.print("/"); Serial.print(year());Serial.print("\n"); 
      Serial.print("Num Of Samples : "); Serial.print(x);Serial.print("\n"); 
      Serial.print("Num  "); Serial.print("Peak values  "); Serial.print("Hr:Min:Sec");Serial.print("\n"); 
      int i = 0,count=1;
      for (int y = 0; y < x; y++) {
        if(count<10){
        Serial.print(count);Serial.print("    ");EEPROM.get(i, load);Serial.print(load, 5);Serial.print("    ");
        Serial.print(EEPROM.read(i + 4));Serial.print(":");Serial.print(EEPROM.read(i + 6));Serial.print(":");Serial.print(EEPROM.read(i + 8));
        Serial.print("\n");}
        if(count>=10){
        Serial.print(count);Serial.print("   ");EEPROM.get(i, load);Serial.print(load, 5);Serial.print("    ");
        Serial.print(EEPROM.read(i + 4));Serial.print(":");Serial.print(EEPROM.read(i + 6));Serial.print(":");Serial.print(EEPROM.read(i + 8));
        Serial.print("\n");}
        
        /*
        Serial.print("sample - load : "); EEPROM.get(i, load);   Serial.println(load, 5);
        Serial.print("sample - Hour : ");   Serial.println(EEPROM.read(i + 4));
        Serial.print("sample - Minute : "); Serial.println(EEPROM.read(i + 6));
        Serial.print("sample - Sec : ");    Serial.println(EEPROM.read(i + 8));
        Serial.println();*/
        i = i + 10;count++;
      }
      Serial.print("Average: ");    Serial.println(stats.average(d, x), 5);
      Serial.print("MAX PEAK: ");    Serial.println(stats.maximum(d, x), 5);
      Serial.print("MIN PEAK: ");    Serial.println(stats.minimum(d, x), 5);
      Serial.print("Standard Deviation: ");    Serial.println(stats.stdev(d, x), 5);
      /*
        char x=EEPROM.read(300);
        infostored st   ored_sample[x];
        for (int i=0;i<=(x-1);i++){
        eeprom_read_block((void*)&stored_sample, (void*)((i*structSize)), structSize);
        Serial.print("sample - load : ");   Serial.println(stored_sample[i].load,5);
        Serial.print("sample - Hour : ");   Serial.println(stored_sample[i].Hr);
        Serial.print("sample - Minute : "); Serial.println(stored_sample[i].Min);
        Serial.print("sample - Sec : ");    Serial.println(stored_sample[i].Sec);*/
    }
  }
  else {}
}
