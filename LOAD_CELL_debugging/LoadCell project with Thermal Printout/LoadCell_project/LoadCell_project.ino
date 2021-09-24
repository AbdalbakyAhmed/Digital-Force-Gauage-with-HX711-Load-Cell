#include "Project.h"     //That's Include every files needed into sketch 
//////// Conncection for HX711 /////////////
#define DOUT  50
#define CLK  51
#define calibration_factor -84864

HX711 scale(DOUT, CLK);
/////////////////////
float Threshold = .001;  //To avoid noise effect "we use it to campare between every Avg"
float Touch_Detect;     //Touch detect in N, value could be changed through the app
float OverLoad_sample; //OverLoad point in N, value could be changed through the app

/////////Used in peak_value_found() ///////////
const byte  arr_samples_size = 70; //Stack width for the LoadCell Readings
const char  NO_sub_samples = 3;   //if you changed 3 then you must change the equation
float arr_samples_readings[arr_samples_size ]; //Stack memo to store Readings
char  reading_counter;//counter used in peak_value_found()
float reading;
float avg_array[arr_samples_size ]; //another stack memo to store the average of Readings to campare
boolean overload = false, peakdone = false, peakdetected = false, touchdetected = false; //some keys used into peak_value_found()
float peakvalue;//value returned from peak_value_found()

///////////////////
float peak_value_found();                   //This Function returns the peak value, if detected !
float get_corrected_value(float value);    //This Function corrects the peak value upon on STD calibration
void Thermal_printer_InKg();
void Thermal_printer_InN();
//////////////////
Keypad customKeypad = Keypad( makeKeymap(Keys), rowPins, colPins, ROWS, COLS);//initialize an instance of class NewKeypad

///////variables used in Store sample in EEPROM//////////
float value; //to peak value after correction
int Hr;
int Min;
int Sec;
int memo_step = 0; //TO store where the Cursor after get reading
QuickStats stats; //To calclualte the math results

//////////used in Password get && change layers///////////
char  password[4]; //array to store already password
char  pass[4], pass1[4]; //to get password and to compare
char  pass_position = 0; // counter to indicate in reading
char  customKey = 0, pass_count = 0;
boolean access = false; //key used into checking the password
void(* resetFunc) (void) = 0; //set Zeros

////////////////////
char Key, Key2, Key3, Key4, Key5, System_Key, Overflow_Key, TP_Key, cal_in_Key, cal_out_Key, CalibChoice; //some Chars used into the app
char  st = 0, first_cle = 0;//flags in system menu
////////used in Printing System///////////
char  Batch_ID[12];
char  ID_count = 0;
////////used in Manual Calibration/////////
float M_cal_input[10], M_cal_output[10]; //two arrays to store from manual Calib
float cal_in_value = 0; //to store Reference value
float cal_out_val = 0;  //to store real value
byte  memo_calb_st = 0; //indicates the arrays memo
////////used in STD caibration////////
float calib_value = 0, old_calib = 0, offset_5, offset_10, offset_15, offset_20, offset_25, offset_30;
float corrected_peak = 0;
/////////////
char end_read;


void setup() {

  Serial.begin(9600);
  lcd.begin(20, 4);
  lcd.clear();
  ///////Thermal printer//////////
  Printer.begin(19200); // to write to our new printer
  initPrinter();
  ////////////////
  pinMode(touchdetectedpin, OUTPUT);
  pinMode(peakdetectedpin, OUTPUT);
  pinMode(overloadpin, OUTPUT);
  pinMode(end_test, INPUT);
  //////////////////
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  //////////////////
  Time_set(); //Sync. for settint correct Time

  ////////to get password from EEPROM/////////
  for (char  j = 0; j < 4; j++) {
    pass[j] = EEPROM.read(pass_EEPROM + j);
  }

  ////////////////Get EEPROM Memories
  memo_step = EEPROM.read(samples_num_EEPROM);    //to set the EEPROM cursor
  EEPROM.get(Overflow_EEPROM, OverLoad_sample);  //Overload EEPROM get
  EEPROM.get(Touch_detect_EEPROM, Touch_Detect);//Touch_Detect EEPROM get
  // for Offsets
  EEPROM.get(layer5_EEPROM, offset_5);
  EEPROM.get(layer10_EEPROM, offset_10);
  EEPROM.get(layer15_EEPROM, offset_15);
  //to get Manual calibration values
  for (char I = 0; I < 10; I++) {
    EEPROM.get(M_cal_input_EEPROM + I * 4, M_cal_input[I]);
    EEPROM.get(M_cal_output_EEPROM + I * 4, M_cal_output[I]);
  }

  //////start with the PINs are LOW//////////
  digitalWrite(touchdetectedpin, LOW);
  digitalWrite(peakdetectedpin, LOW);
  digitalWrite(overloadpin, LOW);

  ///////////////First on LCD
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Manu: EGYEXPERTS.com");
  lcd.setCursor(0, 1); lcd.print("Customer: EAC");
  lcd.setCursor(0, 2); lcd.print("Model: AHT300");
  lcd.setCursor(0, 3); lcd.print(day()); lcd.print("/"); lcd.print(month()); lcd.print("/"); lcd.print(year());
  lcd.print("   "); lcd.print(hour()); lcd.print(":"); lcd.print(minute());
  delay(5000);
  first_cle = 0; //flag to lcd clear once
}


void loop() {
  char hama1 = 0, hama2 = 0;
  end_read = digitalRead(end_test);
  lcd.clear();
  while (end_read == LOW) {
    if (hama1 == 0) {
      lcd.clear();
      delay(200);
      hama1 = 1;
    }
    overload = false, peakdone = false, peakdetected = false, touchdetected = false;
    value = peak_value_found();//peak value in kg
    Serial.println("Old value"); Serial.print(value * gravity); Serial.println();//serial print N

    if (value > (Touch_Detect / gravity) && value < (OverLoad_sample / gravity)) {
      corrected_peak = get_corrected_value(value); //return peak in N after correction by STD calibration
      Serial.println("Calibrated value1"); Serial.print(corrected_peak); Serial.println();

      ///////To correct peak values by manual calibration////////////
      for (char N = 0; N < 10; N++) {
        if (M_cal_input[N] == 0) {
          N = 12;
          break;
        }
        if (corrected_peak < M_cal_input[N + 1] && corrected_peak > M_cal_input[N]) {
          float M1_offset = 0, M2_offset = 0;
          M1_offset = M_cal_input[N] - M_cal_output[N];
          M2_offset = M_cal_input[N + 1] - M_cal_output[N + 1];
          corrected_peak = corrected_peak + ((corrected_peak - M_cal_input[N]) * (M2_offset - M1_offset) / (M_cal_input[N + 1] - M_cal_input[N]) ) + M1_offset;
          N = 11;
          break;
        }
      }

      //////////////////////////
      Serial.println("Calibrated value2"); Serial.print(corrected_peak); Serial.println();//Serial print final peak_value
      EEPROM.put(memo_step, corrected_peak);//store the peak in EEPROM using memo_step cursor
      ////////lcd shows value detected////////////
      lcd.clear(); lcd.setCursor(0, 2);
      lcd.print("Peak Value:"); lcd.print(corrected_peak); lcd.print(" N");
      lcd.setCursor(0, 3);
      lcd.print("           "); lcd.print(corrected_peak / gravity); lcd.print(" Kg");
      digitalWrite(peakdetectedpin, HIGH);//ON peak_led
      digitalWrite(touchdetectedpin, LOW);//off touch_detected led
      delay(1000);
      digitalWrite(peakdetectedpin, LOW);
      digitalWrite(touchdetectedpin, LOW);

      /////store the time of sample after 4 bytes in EEPROM/////
      EEPROM.write(memo_step + 4, hour());
      EEPROM.write(memo_step + 6, minute());
      EEPROM.write(memo_step + 8, second());
      memo_step = memo_step + 10;//10 byte step for new sample
      EEPROM.update(samples_num_EEPROM, (memo_step / 10)); //update value in EEPROM
    }
    ///////for Overload detected////////////
    else if (value >= (OverLoad_sample / gravity)) {
      lcd.clear();
      for (char  i = 0; i < 3; i++) {
        lcd.setCursor(0, 0); lcd.print("OverLoad Detected!");
        lcd.setCursor(0, 1); lcd.print(value * gravity); lcd.print("  N");
        digitalWrite(overloadpin, HIGH);
        delay(1000);
        digitalWrite(overloadpin, LOW);
        lcd.clear();
        delay(1000);
      }
      digitalWrite(touchdetectedpin, LOW);
    }
    ///////to break the loop to get the system menu/////////////
    end_read = digitalRead(end_test);
    if (end_read == HIGH) {
      lcd.clear();
      hama2 = 0;
      break;
    }
  }

  //////////////////////Application///////////////////////////////////

  while (end_read == HIGH) {
    if (hama2 == 0) {
      lcd.clear();
      delay(200);
      hama2 = 1;
    }
    if (first_cle == 0) {

      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Manu: EGYEXPERTS.com");
      lcd.setCursor(0, 2); lcd.print("- Enter : CAL or SET");
      lcd.setCursor(0, 3); lcd.print("- Print Samples");
      first_cle = 1;
    }
    //"""""""" Printing System """"""""\\
    char  st = 0; //counter for steps on App scrolling
    Key = customKeypad.getKey();
    //if we press print firstly
    char  k = 0;
    while (Key == 'P') {
      int get_N = EEPROM.read(samples_num_EEPROM); //to get num of samples in EEPROM
      if (get_N >= 0) {
        if (k == 0) {
          lcd.clear();
          k = 1;
        }
        lcd.setCursor(0, 0); lcd.print("Enter Batch ID :");
        Key3 = customKeypad.getKey();
        if (Key3 == '0' || Key3 == '1' || Key3 == '2' || Key3 == '3' || Key3 == '4'
            || Key3 == '5' || Key3 == '6' || Key3 == '7' || Key3 == '8' || Key3 == '9') {
          Batch_ID[ID_count] = Key3; ID_count++;
          lcd.setCursor(ID_count - 1, 1); lcd.print(Key3);
        }
        if (Key3 == 'B') {
          ID_count = 0; first_cle = 0;
          break;
        }
        end_read = digitalRead(end_test);
        if (end_read == LOW) {
          lcd.clear();
          first_cle = 0;
          ID_count = 0;
          break;
        }
        byte ID_cle = 0;
        while (ID_count == 12) {
          if (ID_cle == 0) {
            delay(2000); lcd.clear(); delay(200); ID_cle = 1;
          }
          lcd.setCursor(0, 0); lcd.print("Press:");
          lcd.setCursor(0, 1); lcd.print("1.To print in Kg");
          lcd.setCursor(0, 2); lcd.print("2.To print in N");
          lcd.setCursor(0, 3); lcd.print("- Back");

          Key4 = customKeypad.getKey();

          if (Key4 == '1') {
            delay(500);
            /////////////Thermal Print_Out in Kg////////////////////
            Thermal_printer_InKg();//this Functions call Printer function to print
            ////////////////////////////////
            end_read = digitalRead(end_test);
            if (end_read == LOW) {
              lcd.clear();
              first_cle = 0;
              break;
            }

          }

          else if (Key4 == '2') {
            delay(500);
            ///////////Thermal Print_Out in N///////////////////
            Thermal_printer_InN();//This Functions call printer functions to print
            ////////////////////////////////
          }
          else if (Key4 == 'B') {
            ID_count = 0; lcd.clear(); delay(100); break;
          }
          end_read = digitalRead(end_test);
          if (end_read == LOW) {
            lcd.clear();
            first_cle = 0;
            break;
          }
        }
      }
    }

    ////////////////Enter System////////////

    char sys_cle = 0;
    while (Key == 'E') {
      if (sys_cle == 0) {
        lcd.clear();
        delay(100);
        sys_cle = 1;
      }

      lcd.print(" Settings");
      lcd.setCursor(0, 1);
      lcd.print(" Calibration Report");
      Key2 = customKeypad.getKey();
      if (Key2 == 'D') {
        st++;
      }
      else if (Key2 == 'U') {
        st = 0;
      }
      else {}
      if (st == 0) {
        lcd.setCursor(0, 0);
        lcd.blink();
        delay(50);
      }
      else if (st >= 1) {
        lcd.setCursor(0, 1);
        lcd.blink();
        delay(50);
      }
      else {}
      //Key2 = customKeypad.getKey();
      char  l = 0;
      while (Key2 == 'E' && st == 0) {

        /* Password system */

        if (l == 0) {
          lcd.clear();
          delay(200);
          l++;
        }
        lcd.setCursor(0, 0); lcd.print("Passcode Please:");
        customKey = customKeypad.getKey();
        if (customKey == '0' || customKey == '2' || customKey == '1' || customKey == '3' || customKey == '4'
            || customKey == '5' || customKey == '6' || customKey == '7' || customKey == '8' || customKey == '9' || customKey == 'B') {
          if (customKey == 'B') {
            pass_count = 0;
            lcd.clear();
            delay(100); sys_cle = 0;
            break;
          }
          if (customKey != 'B') {
            password[pass_count++] = customKey;
            lcd.setCursor(pass_count - 1, 1); lcd.print(customKey);
          }
        }
        if (pass_count == 4) {
          pass_count = 0;
          delay(200);
          for (char  j = 0; j < 4; j++) {
            pass[j] = EEPROM.read(pass_EEPROM + j);
          }
          char  p = 0;
          /////Password is correct//////
          if (!(strncmp(password, pass, 4) )) {
            pass_count = 0;
            while (!(strncmp(password, pass, 4) )) {
              if (p == 0) {
                lcd.clear();
                lcd.print("Passkey Accepted!");
                delay(2000);
                lcd.clear();
                p++;
              }
              lcd.setCursor(0, 0); lcd.print("Press: 1.change pass");
              lcd.setCursor(0, 1); lcd.print("2.OverLoad value");
              lcd.setCursor(0, 2); lcd.print("3.TouchValue value");
              lcd.setCursor(0, 3); lcd.print("4.Calibration Mode");
              System_Key = customKeypad.getKey();
              if (System_Key == '1') {
                change();//function to change the password
              }
              if (System_Key == 'B') {
                Key2 = 'B';
                Key = 'E';
                lcd.clear();
                sys_cle = 0;
                //for(int I;I<4;I++){pass[I]=I+2;password[I]=I+1;}
                st = 0; break;
              }



              /////For OverFlow change/////
              char  ov_cl = 0;
              while (System_Key == '2') {
                if (ov_cl == 0) {
                  lcd.clear();
                  ov_cl = 1;
                }
                System_Key = customKeypad.getKey();
                lcd.setCursor(0, 0); lcd.print("Current Value:");
                float OverFlow_value;
                EEPROM.get(Overflow_EEPROM, OverFlow_value);//to get OverLOad_Value from EEPROM
                lcd.setCursor(0, 1); lcd.print(OverFlow_value);
                OverFlow_value = 0;
                lcd.setCursor(0, 3); lcd.print("Press 1. to change");
                char  ov2_cl = 0, ov3_cl = 0, sa_cl = 0, ov_st = 0, dot_flag = 0, add_dot_flag = 0, dot_count = 0;//some flags to control the loop
                while (System_Key == '1') {
                  if (ov3_cl == 0) {
                    lcd.clear();
                    ov3_cl = 1;
                  }
                  lcd.setCursor(0, 0); lcd.print("Enter Value");
                  lcd.setCursor(0, 2); lcd.print("Press Enter TO SAVE");
                  lcd.setCursor(0, 3); lcd.print("OR Cancel TO BACK");
                  Overflow_Key = customKeypad.getKey();
                  if (Overflow_Key == '0' || Overflow_Key == '2' || Overflow_Key == '1' || Overflow_Key == '3' || Overflow_Key == '4'
                      || Overflow_Key == '5' || Overflow_Key == '6' || Overflow_Key == '7' || Overflow_Key == '8' || Overflow_Key == '9' || Overflow_Key == '.') {

                    lcd.setCursor(ov_st, 1); lcd.print(Overflow_Key); ov_st++;
                    if (Overflow_Key == '.') {
                      dot_flag = 1;
                      dot_count = 0;
                    }
                    if (dot_flag == 0 && Overflow_Key != '.') {
                      OverFlow_value = OverFlow_value * 10 + (Overflow_Key - '0');
                    }
                    else if (dot_flag == 1 && Overflow_Key != '.') {
                      dot_count++;
                      OverFlow_value = (OverFlow_value * 10 + (Overflow_Key - '0')) / (10 * dot_count);
                      //float new_Key = (Overflow_Key - '0') / (dot_count * 10);
                      //OverFlow_value = OverFlow_value + new_Key;
                    }
                    /*if(Overflow_Key!='.'&&add_dot_flag!=1){
                      dot_flag=1;
                      OverFlow_value = OverFlow_value * 10 + (Overflow_Key - '0');
                      lcd.setCursor(ov_st, 1); lcd.print(Overflow_Key); ov_st++;}
                      else if(Overflow_Key=='.'){
                      add_dot_flag=1;lcd.setCursor(ov_st, 1); lcd.print(Overflow_Key); ov_st++;}
                      else if(add_dot_flag==1&&dot_flag!=1){
                      dot_count++;
                      OverFlow_value=(OverFlow_value*10+(Overflow_Key-'0'))/(10*dot_count);
                      lcd.setCursor(ov_st, 1); lcd.print(Overflow_Key); ov_st++;}
                      else if(dot_flag==1&&add_dot_flag==1){
                      dot_count++;
                      OverFlow_value=OverFlow_value+((OverFlow_value*10+(Overflow_Key-'0'))/(10*dot_count));}*/
                  }
                  else if (Overflow_Key == 'E') {
                    if (sa_cl == 0) {
                      lcd.clear();
                      delay(1000);
                      sa_cl = 1;
                    }
                    lcd.setCursor(0, 0); lcd.print("OverLoad Value:");
                    lcd.setCursor(0, 1); lcd.print(OverFlow_value);
                    EEPROM.put(Overflow_EEPROM, OverFlow_value);
                    delay(2000);
                    break;
                  }
                  else if (Overflow_Key == 'B') {
                    lcd.clear(); delay(100);
                    break;
                  }
                }
                if (System_Key != 'B') {
                  System_Key = '2';
                }
                else {
                  lcd.clear();
                  delay(500);
                  break;
                }
                end_read = digitalRead(end_test);
                if (end_read == LOW) {
                  lcd.clear();
                  first_cle = 0;;
                  break;
                }

              }



              /////For Touch Point change/////
              char  TP_cl = 0;
              while (System_Key == '3') {
                if (TP_cl == 0) {
                  lcd.clear();
                  TP_cl = 1;
                }
                System_Key = customKeypad.getKey();
                lcd.setCursor(0, 0); lcd.print("Current Value:");
                float TP_value;
                EEPROM.get(Touch_detect_EEPROM, TP_value);//to get Touch_detect value from EEPROM
                lcd.setCursor(0, 1); lcd.print(TP_value);
                TP_value = 0;
                lcd.setCursor(0, 3); lcd.print("Press 1. to change");
                char  TP2_cl = 0, TP3_cl = 0, sa_cl = 0, TP_st = 0, dot_flag = 0, dot_count = 0;//some flags to control the loop
                while (System_Key == '1') {
                  if (TP3_cl == 0) {
                    lcd.clear();
                    TP3_cl = 1;
                  }
                  lcd.setCursor(0, 0); lcd.print("Enter Value");
                  lcd.setCursor(0, 2); lcd.print("Press Enter TO SAVE");
                  lcd.setCursor(0, 3); lcd.print("OR Cancel TO BACK");
                  TP_Key = customKeypad.getKey();
                  if (TP_Key == '0' || TP_Key == '2' || TP_Key == '1' || TP_Key == '3' || TP_Key == '4'
                      || TP_Key == '5' || TP_Key == '6' || TP_Key == '7' || TP_Key == '8' || TP_Key == '9' || TP_Key == '.') {

                    lcd.setCursor(TP_st, 1); lcd.print(TP_Key); TP_st++;
                    if (TP_Key == '.') {
                      dot_flag = 1;
                      dot_count = 0;
                    }
                    if (dot_flag == 0 && TP_Key != '.') {
                      TP_value = TP_value * 10 + (TP_Key - '0');
                    }
                    else if (dot_flag == 1 && TP_Key != '.') {
                      dot_count++;
                      TP_value = (TP_value * 10 + (TP_Key - '0')) / (10 * dot_count);
                      //float new_Key = (TP_Key - '0') / (dot_count * 10);
                      //TP_value = TP_value + new_Key;
                    }
                  }
                  else if (TP_Key == 'E') {
                    if (sa_cl == 0) {
                      lcd.clear();
                      delay(1000);
                      sa_cl = 1;
                    }
                    lcd.setCursor(0, 0); lcd.print("Touch Value:");
                    lcd.setCursor(0, 1); lcd.print(TP_value);
                    EEPROM.put(Touch_detect_EEPROM, TP_value);//to save the NEW value
                    delay(2000);
                    break;
                  }
                  else if (TP_Key == 'B') {
                    lcd.clear(); delay(100);
                    break;
                  }
                }
                if (System_Key != 'B') {
                  System_Key = '3';
                }
                else {
                  lcd.clear();
                  delay(500);
                  break;
                }
                end_read = digitalRead(end_test);
                if (end_read == LOW) {
                  lcd.clear();
                  first_cle = 0;;
                  break;
                }
              }



              /////////Clibratiom Mode//////////
              char  CM_cl = 0, calb_st = 0, dot_flag1 = 0, dot_count1 = 0;//some flags to control the loop
              while (System_Key == '4') {
                if (CM_cl == 0) {
                  lcd.clear(); delay(100); CM_cl = 1;
                }
                byte Cursor = EEPROM.read(Manual_calib_cursor_EEPROM);//to get the array position from EEPROM
                if (Cursor > 10) {//to clear the readings in array after 10 readings
                  EEPROM.write(Manual_calib_cursor_EEPROM, 1);
                  Cursor = 1;
                  EEPROM.put(M_cal_input_EEPROM , 0);
                  EEPROM.put(M_cal_output_EEPROM, 0);
                }
                lcd.setCursor(0, 0); lcd.print("Enter Ref "); lcd.print(Cursor); lcd.print(" :");
                lcd.setCursor(0, 2); lcd.print("press Enter TO SAVE");
                lcd.setCursor(0, 3); lcd.print("press Back  TO CANCEL");
                cal_in_Key = customKeypad.getKey();
                if (cal_in_Key == '0' || cal_in_Key == '2' || cal_in_Key == '1' || cal_in_Key == '3' || cal_in_Key == '4'
                    || cal_in_Key == '5' || cal_in_Key == '6' || cal_in_Key == '7' || cal_in_Key == '8' || cal_in_Key == '9' || cal_in_Key == '.') {

                  lcd.setCursor(calb_st, 1); lcd.print(cal_in_Key); calb_st++;
                  if (cal_in_Key == '.') {
                    dot_flag1 = 1;
                    dot_count1 = 0;
                  }
                  else if (dot_flag1 == 0 && cal_in_Key != '.') {
                    cal_in_value = cal_in_value * 10 + (cal_in_Key - '0');
                  }
                  else if (dot_flag1 == 1 && cal_in_Key != '.') {
                    (dot_count1)++;
                    cal_in_value = (cal_in_value * 10 + (cal_in_Key - '0')) / (10 * dot_count1);
                    //float new_Key = (cal_in_Key - '0') / (dot_count * 10);
                    //cal_in_value = cal_in_value + new_Key;
                  }
                }


                char  sa_cl;
                if (cal_in_Key == 'E') {
                  if (sa_cl == 0) {
                    lcd.clear(); delay(100); sa_cl = 1;
                  }
                  CM_cl = 0, calb_st = 0, dot_flag1 = 1, dot_count1 = 0;//Zero the flags for the Entered values
                  lcd.setCursor(0, 0); lcd.print("Entered Value:");
                  lcd.setCursor(0, 1); lcd.print(cal_in_value);
                  memo_calb_st = EEPROM.read(cal_st_EEPROM);//get calib step from EEPROM
                  EEPROM.put(M_cal_input_EEPROM + (memo_calb_st * 4), cal_in_value);//store the new value into EEPROM with 4 byte Step
                  //memo_calb_st = memo_calb_st +1; EEPROM.write(cal_st_EEPROM, memo_calb_st);
                  cal_in_value = 0;//zero the i/p variable
                  delay(1000); lcd.clear();
                  cal_out_val = 0;//zero the o/p variable
                  char  CM2_cl = 0, cal_st = 0, dot_flag2 = 0, dot_count2 = 0;//some flags to control the loop
                  while (cal_in_Key == 'E') {
                    if (CM2_cl == 0) {
                      lcd.clear(); delay(100); CM2_cl = 1;
                    }

                    lcd.setCursor(0, 0); lcd.print("Measured "); lcd.print(Cursor); lcd.print(" Value");
                    lcd.setCursor(0, 2); lcd.print("press Enter TO SAVE");
                    lcd.setCursor(0, 3); lcd.print("press Back to CANCEL");
                    cal_out_Key = customKeypad.getKey();
                    if (cal_out_Key == '0' || cal_out_Key == '2' || cal_out_Key == '1' || cal_out_Key == '3' || cal_out_Key == '4'
                        || cal_out_Key == '5' || cal_out_Key == '6' || cal_out_Key == '7' || cal_out_Key == '8' || cal_out_Key == '9' || cal_out_Key == '.') {
                      lcd.setCursor(cal_st, 1); lcd.print(cal_out_Key); cal_st++;
                      if (cal_out_Key == '.') {
                        dot_flag2 = 1;
                        dot_count2 = 0;
                      }
                      else if (dot_flag2 == 0 && cal_out_Key != '.') {
                        cal_out_val = cal_out_val * 10 + (cal_out_Key - '0');
                      }
                      else if (dot_flag2 == 1 && cal_out_Key != '.') {
                        dot_count2++;
                        cal_out_val = (cal_out_val * 10 + (cal_out_Key - '0')) / (10 * dot_count2);
                        //float new_Key = (cal_out_Key - '0') / (dot_count * 10);
                        //cal_out_val = cal_out_val + new_Key;
                      }
                      else {}
                    }
                    char  sa_cl = 0;
                    if (cal_out_Key == 'E') {
                      if (sa_cl == 0) {
                        lcd.clear(); delay(1000); sa_cl = 1;
                      }
                      Cursor = Cursor + 1;
                      EEPROM.write(Manual_calib_cursor_EEPROM, Cursor);//save the array position with 1 byte step
                      lcd.setCursor(0, 0); lcd.print("Instead OF :");
                      lcd.setCursor(0, 1); lcd.print(cal_out_val);
                      //memo_calb_st = memo_calb_st + 1;
                      EEPROM.put(M_cal_output_EEPROM + (memo_calb_st * 4), cal_out_val);//save the measured value in EEPROM with 4 byte step
                      memo_calb_st = memo_calb_st + 1; EEPROM.write(cal_st_EEPROM, memo_calb_st);//save the memo step in EEPROM after add 1
                      delay(1000); lcd.clear();
                      //cal_out_val=0;
                      delay(2000); break;

                    }
                    else if (cal_out_Key == 'B') {
                      //to zero last memo
                      EEPROM.put(M_cal_input_EEPROM  + (memo_calb_st * 4), 0);
                      EEPROM.put(M_cal_output_EEPROM + (memo_calb_st * 4), 0);
                      //memo_calb_st = memo_calb_st - 1; EEPROM.write(cal_st_EEPROM, memo_calb_st);
                      //cal_out_val=0;
                      lcd.clear();
                      break;
                    }
                    else {}
                  }
                  lcd.clear();
                  break;
                }
                if (cal_in_Key == 'B') {
                  //to zero last memo
                  EEPROM.put(M_cal_input_EEPROM +  (memo_calb_st * 4), 0);
                  EEPROM.put(M_cal_output_EEPROM + (memo_calb_st * 4), 0);
                  //EEPROM.write(cal_st_EEPROM, memo_calb_st);
                  lcd.clear();
                  calb_st = 0; cal_in_value = 0;
                  break;
                }
                end_read = digitalRead(end_test);
                if (end_read == LOW) {
                  lcd.clear();
                  first_cle = 0;;
                  break;
                }
              }
              end_read = digitalRead(end_test);
              if (end_read == LOW) {
                lcd.clear();
                first_cle = 0;;
                break;
              }

            }
            end_read = digitalRead(end_test);
            if (end_read == LOW) {
              lcd.clear();
              first_cle = 0;;
              break;
            }
          }
          //if the password is wrong
          else {
            lcd.clear();
            lcd.print("Access Denied..."); delay(2000);
            pass_count = 0; lcd.clear(); delay(500); break; //resetFunc();
          }
          end_read = digitalRead(end_test);
          if (end_read == LOW) {
            lcd.clear();
            first_cle = 0;;
            break;
          }

        }
        end_read = digitalRead(end_test);
        if (end_read == LOW) {
          lcd.clear();
          first_cle = 0;;
          break;
        }


      }


      char  cal_cle = 0, back_flag = 0;
      while (Key2 == 'E' && st >= 1) {
        if (cal_cle == 0) {
          lcd.clear();
          delay(500);
          cal_cle = 1;
        }
        end_read = digitalRead(end_test);
        if (end_read == LOW) {
          lcd.clear();
          first_cle = 0;;
          break;
        }
        lcd.setCursor(0, 0);
        lcd.print("Calibration Menu");
        lcd.setCursor(0, 1);
        lcd.print("press 1 : 5  Kg");
        lcd.setCursor(0, 2);
        lcd.print("press 2 : 10 Kg");
        lcd.setCursor(0, 3);
        lcd.print("press 3 : 15 Kg");
        Key5 = customKeypad.getKey();
        char  cal_5 = 0;
        while (Key5 == '1') {
          if (cal_5 == 0) {
            lcd.clear();
            delay(100);
            cal_5 = 1;
            lcd.setCursor(0, 0); lcd.print("Please put 5Kg STD");
            lcd.setCursor(0, 1); lcd.print("During 10 Sec");
          }
          delay(calibration_time);
          //get weight from Loadcell
          scale.set_scale(calibration_factor);
          calib_value = scale.get_units(15); delay(100);
          //check for noise value
          if ((calib_value >= 0 && calib_value <= Touch_Detect)) {
            lcd.setCursor(0, 0);
            lcd.print("Please put 5Kg STD");
            lcd.setCursor(0, 2);
            lcd.print(calib_value);
            delay(1000);
            break;
          }
          //if value detected
          else if ((calib_value < 5.5 && calib_value > 4.5)) {
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print(calib_value); delay(2000);
            delay(1500);
            lcd.setCursor(0, 1); lcd.print("sample's calibrated.");
            delay(1500);
            offset_5 = 5 - calib_value;
            lcd.setCursor(0, 3); lcd.print("Offset_5= "); lcd.print(offset_5);
            delay(4000);
            EEPROM.put(layer5_EEPROM, offset_5);
            lcd.clear();
            break;
          }
          //if value < STD weight and > STD weight
          else if ((calib_value > Touch_Detect && calib_value < 4.5) || (calib_value > Touch_Detect && calib_value > 5.5)) {
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("Wrong STD !!!");
            lcd.setCursor(0, 1); lcd.print(calib_value);
            delay(3000);
            cal_5 = 0;
            break;
          }


        }
        char  cal_10 = 0;
        while (Key5 == '2') {
          if (cal_10 == 0) {
            lcd.clear();
            delay(100);
            cal_10 = 1;
            lcd.setCursor(0, 0); lcd.print("Please put 10Kg STD");
            lcd.setCursor(0, 1); lcd.print("During 10 Sec");
          }
          delay(calibration_time);
          //get weight from Loadcell
          scale.set_scale(calibration_factor);
          calib_value = scale.get_units(15); delay(100);
          //check for noise value
          if ((calib_value >= 0 && calib_value <= Touch_Detect)) {
            lcd.setCursor(0, 0);
            lcd.print("Please put 10Kg STD");
            lcd.setCursor(0, 2);
            lcd.print(calib_value);
            delay(1000);
            break;
          }
          //if value detected
          else if ((calib_value < 10.5 && calib_value > 9.5)) {
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print(calib_value); delay(2000);
            delay(1500);
            lcd.setCursor(0, 1); lcd.print("sample's calibrated.");
            delay(1500);
            offset_10 = 10 - calib_value;
            lcd.setCursor(0, 3); lcd.print("Offset_10= "); lcd.print(offset_10);
            delay(4000);
            //offset_5=(calib_value+((calib_value-0)*(((calib_value-5)-0)/(5-0)))+0)-5;
            EEPROM.put(layer10_EEPROM, offset_10);
            lcd.clear();
            break;
          }
          //if value < STD weight and > STD weight
          else if ((calib_value > Touch_Detect && calib_value < 9.5) || (calib_value > Touch_Detect && calib_value > 10.5)) {
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("Wrong STD !!!");
            lcd.setCursor(0, 1); lcd.print(calib_value);
            delay(3000);
            cal_10 = 0;
            break;
          }
        }
        char  cal_15 = 0;
        while (Key5 == '3') {
          if (cal_15 == 0) {
            lcd.clear();
            delay(100);
            cal_15 = 1;
            lcd.setCursor(0, 0); lcd.print("Please put 15Kg STD");
            lcd.setCursor(0, 1); lcd.print("During 10 Sec");
          }
          delay(calibration_time);
          //get weight from Loadcell
          scale.set_scale(calibration_factor);
          calib_value = scale.get_units(15); delay(100);
          //check for noise value
          if ((calib_value >= 0 && calib_value <= Touch_Detect)) {
            lcd.setCursor(0, 0);
            lcd.print("Please put 15Kg STD");
            lcd.setCursor(0, 2);
            lcd.print(calib_value);
            delay(1000);
            break;
          }
          //if value detected
          else if ((calib_value < 15.5 && calib_value > 14.5)) {
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print(calib_value); delay(2000);
            delay(1500);
            lcd.setCursor(0, 1); lcd.print("sample's calibrated.");
            delay(1500);
            offset_15 = 15 - calib_value;
            lcd.setCursor(0, 3); lcd.print("Offset_15= "); lcd.print(offset_15);
            delay(4000);
            //offset_5=(calib_value+((calib_value-0)*(((calib_value-5)-0)/(5-0)))+0)-5;
            EEPROM.put(layer15_EEPROM, offset_15);
            lcd.clear();
            break;
          }
          //if value < STD weight and > STD weight
          else if ((calib_value > Touch_Detect && calib_value < 14.5) || (calib_value > Touch_Detect && calib_value > 15.5)) {
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("Wrong STD !!!");
            lcd.setCursor(0, 1); lcd.print(calib_value);
            delay(3000);
            cal_15 = 0;
            break;
          }
        }
        if (Key5 == 'B') {
          st = 0;
          sys_cle = 0;
          break;
        }
      }
      if (Key2 != 'B') {
        Key = 'E';
      }
      else if (Key2 == 'B') {
        lcd.clear(); delay(100);
        first_cle = 0; lcd.setCursor(0, 0); break;
      }
      end_read = digitalRead(end_test);
      if (end_read == LOW) {
        lcd.clear();
        first_cle = 0;;
        break;
      }
    }
    end_read = digitalRead(end_test);
    if (end_read == LOW) {
      lcd.clear();
      first_cle = 0;;
      break;
    }
  }
  ///////////////////
  digitalWrite(touchdetectedpin, LOW);
  digitalWrite(peakdetectedpin, LOW);
  digitalWrite(overloadpin, LOW);
}

/////////////////////
//This Function retuns peak value after correction by STD offsets calibration


float get_corrected_value (float value) {
  float reading_after = 0;
  Serial.println("Old value"); Serial.print(value * gravity); Serial.println();
  if (Touch_Detect < value < 5) {
    reading_after = value + ((value - 0) * (offset_5 - 0) / (5 - 0) ) + 0;
  }

  if (5 < value < 10) {
    reading_after = value + ((value - 5) * (offset_10 - offset_5) / (10 - 5) ) + offset_5;
  }

  if (10 < value < 15) {
    reading_after = value + ((value - 10) * (offset_15 - offset_10) / (15 - 10) ) + offset_10;
  }
  /*
    if (15 < value < 20) {
    reading_after = value + ((value - 15) * (offset_20 - offset_15) / (20 - 15) ) + offset_15;
    }

    if (20 < value < 25) {
    reading_after = value + ((value - 20) * (offset_25 - offset_20) / (25 - 20) ) + offset_20;
    }

    if (25 < value < 30) {
    reading_after = value + ((value - 25) * (offset_30 - offset_25) / (30 - 25) ) + offset_25;
    }
  */
  return (reading_after * gravity);
}


///////////////////////////////////////
//This Function returns the peak value, if detected !

float peak_value_found() {
  float exact_peak_value = 0;
  //overload = false, peakdone = false, peakdetected = false, touchdetected = false;
  for (reading_counter = 0; reading_counter < arr_samples_size && peakdetected == false; reading_counter++)
  {
    scale.set_scale(calibration_factor);
    float weight = scale.get_units(1);
    reading = weight;
    lcd.setCursor(0, 0); lcd.print(reading); lcd.setCursor(11, 0); lcd.print(reading * gravity); //lcd.print(" N");
    lcd.setCursor(7, 0); lcd.print("Kg"); lcd.setCursor(18, 0); lcd.print("N");
    arr_samples_readings[reading_counter] = reading;
    if ( arr_samples_readings[reading_counter] >= (Touch_Detect / 9.81) && touchdetected == false) {
      touchdetected = true; digitalWrite(touchdetectedpin, HIGH);
    }

    if (reading_counter >= NO_sub_samples) {
      avg_array[reading_counter] = (arr_samples_readings[reading_counter] + arr_samples_readings[reading_counter - 1] + arr_samples_readings[reading_counter - 2]) / NO_sub_samples;
      if (avg_array[reading_counter] < (avg_array[reading_counter - 1] - (Threshold / 9.81) ) && reading > Touch_Detect ) //comparing last two averages stored in samples array
      {
        peakdetected = true;
        peakvalue = arr_samples_readings[reading_counter - 1];
        if (peakdetected == true && peakdone == false) {
          char  k = reading_counter;
          exact_peak_value = arr_samples_readings[k];
          for (char i = reading_counter; i >= 0 ; i--) {
            if (arr_samples_readings[i] > exact_peak_value)//exact_peak_value is temporarly storage of temporal peak value till finishing check process.
            {
              exact_peak_value = arr_samples_readings[i];
              k = i; double  exact_peak_value = arr_samples_readings[k];
            }
          }
          peakdone = true;
          break;
        }
      }
    }
  }
  lcd.clear();
  return exact_peak_value;
}
////////////////////////////////
/// This function to change the password from user

void change() {
  int j = 0;
  lcd.clear();
  lcd.print("Current Passcode:");
  lcd.setCursor(0, 1);
  while (j < 4) {
    char  key = customKeypad.getKey();
    if (key == '0' || key == '2' || key == '1' || key == '3' || key == '4'
        || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == 'B') {
      if (key == 'B') {
        j = 0;
        lcd.clear();
        delay(100);
        break;
      }
      pass1[j++] = key;
      lcd.print(key);
    }
    key = 0;
  }
  delay(500);
  if ((strncmp(pass1, pass, 4))) {
    lcd.clear();
    lcd.print("Wrong Passkey...");
    lcd.setCursor(1, 1);
    lcd.print("DENIED");
    delay(2000); lcd.clear();
    System_Key = 0;
  }
  else {
    j = 0;
    lcd.clear();
    lcd.print("New Passcode:");
    lcd.setCursor(0, 1);
    while (j < 4) {
      char  key = customKeypad.getKey();
      if (key == '0' || key == '2' || key == '1' || key == '3' || key == '4'
          || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == 'B') {
        if (key == 'B') {
          j = 0;
          lcd.clear();
          delay(100);
          break;
        }
        if (key != 'B') {
          pass[j] = key;
          lcd.print(key);
          EEPROM.write(pass_EEPROM + j, key);
          j++;
        }
      }
    }
    if (Key == 'B') {
      lcd.setCursor(0, 0);
      lcd.clear();
    }
    if (j == 4) {
      lcd.print("  Done......");
      delay(2000); lcd.clear();
    }
  }
}
//////////////////////////////////////////
//Print Functions

void Thermal_printer_InKg() {
  int x = EEPROM.read(samples_num_EEPROM);//to get numbers of samples in EEPROM
  //////for maths equations///////
  float math_operationts[x]; int cou = 0; float data;
  for (int z = 0; z <= x; z++) {
    EEPROM.get(cou, data); math_operationts[z] = (data / gravity);
    cou = cou + 10;
  }

  // bold text
  Printer.write(27);
  Printer.write(32);
  Printer.write(1);
  delay(100);
  ///////////
  Printer.println();
  Printer.print("Manufacturer : "); Printer.println();
  Printer.print("Customer : "); Printer.println();
  Printer.print("Batch ID : "); for (char p = 0; p <= ID_count; p++) {
    Printer.print(Batch_ID[p]);
  } Printer.println();
  Printer.print("Date : "); Printer.print(day()); Printer.print("/"); Printer.print(month()); Printer.print("/"); Printer.print(year()); Printer.println();
  Printer.print("Num Of Samples : "); Printer.print(x); Printer.println();
  Printer.print("Num  "); Printer.print("Peak values  "); Printer.print("Hr:Min:Sec"); Printer.println();
  // bold text off
  Printer.write(27);
  Printer.write(32);
  Printer.write(zero);
  delay(100);
  Printer.write(10);
  //////////////
  Printer.write(27);
  Printer.write(45);
  Printer.write(zero);
  delay(100);
  Printer.write(10);
  int i = 0, count = 1; float load = 0;
  for (int y = 0; y < x; y++) {
    //check for adjusting the print out
    if (count < 10) {
      EEPROM.get(i, load);
      if ((load / gravity) < 10) {
        Printer.print(count); Printer.print("         "); Printer.print((load / gravity), 5); Printer.print("         ");
      }
      else if ((load / gravity) >= 10 && (load / gravity) < 100) {
        Printer.print(count); Printer.print("         "); Printer.print((load / gravity), 5); Printer.print("        ");
      }
      else if ((load / gravity) >= 100) {
        Printer.print(count); Printer.print("         "); Printer.print((load / gravity), 5); Printer.print("       ");
      }

      //Printer.print(count); Printer.print("         "); EEPROM.get(i, load); Printer.print((load / gravity), 5); Printer.print("         ");

      Printer.print(EEPROM.read(i + 4)); Printer.print(":"); Printer.print(EEPROM.read(i + 6)); Printer.print(":"); Printer.print(EEPROM.read(i + 8));
      Printer.println();
    }
    //check for adjusting the print out
    if (count >= 10) {
      EEPROM.get(i, load);
      if ((load / gravity) < 10) {
        Printer.print(count); Printer.print("        "); Printer.print((load / gravity), 5); Printer.print("         ");
      }
      else if ((load / gravity) >= 10 && (load / gravity) < 100) {
        Printer.print(count); Printer.print("        "); Printer.print((load / gravity), 5); Printer.print("        ");
      }
      else if ((load / gravity) >= 100) {
        Printer.print(count); Printer.print("        "); Printer.print((load / gravity), 5); Printer.print("       ");
      }

      //Printer.print(count); Printer.print("        "); EEPROM.get(i, load); Printer.print(load / gravity, 5); Printer.print("         ");

      Printer.print(EEPROM.read(i + 4)); Printer.print(":"); Printer.print(EEPROM.read(i + 6)); Printer.print(":"); Printer.print(EEPROM.read(i + 8));
      Printer.println();
    }
    i = i + 10; count++;
  }
  // bold text
  Printer.write(27);
  Printer.write(32);
  Printer.write(1);
  delay(100);
  Printer.print("Average: ");    Printer.println(stats.average(math_operationts, x), 5);
  Printer.print("MAX PEAK: ");    Printer.println(stats.maximum(math_operationts, x), 5);
  Printer.print("MIN PEAK: ");    Printer.println(stats.minimum(math_operationts, x), 5);
  Printer.print("Standard Deviation: ");    Printer.println(stats.stdev(math_operationts, x), 5);
  EEPROM.write(samples_num_EEPROM, 0); //for new Batch
}

////////////////////////////////////

void Thermal_printer_InN() {
  int x = EEPROM.read(samples_num_EEPROM);//to get number of samples
  //////for maths equations/////
  float math_operationts[x]; int cou = 0; float data;
  for (int z = 0; z <= x; z++) {
    EEPROM.get(cou, data); math_operationts[z] = data;
    cou = cou + 10;
  }

  Printer.write(27);
  Printer.write(32);
  Printer.write(1);
  delay(100);
  ///////////
  Printer.println();
  Printer.print("Manufacturer : "); Printer.println();
  Printer.print("Customer : "); Printer.println();
  Printer.print("Batch ID : "); for (char p = 0; p <= ID_count; p++) {
    Printer.print(Batch_ID[p]);
  } Printer.println();
  Printer.print("Date : "); Printer.print(day()); Printer.print("/"); Printer.print(month()); Printer.print("/"); Printer.print(year()); Printer.println();
  Printer.print("Num Of Samples : "); Printer.print(x); Printer.println();
  Printer.print("Num  "); Printer.print("Peak values  "); Printer.print("Hr:Min:Sec"); Printer.println();
  // bold text off
  Printer.write(27);
  Printer.write(32);
  Printer.write(zero);
  delay(100);
  Printer.write(10);
  //////////////
  Printer.write(27);
  Printer.write(45);
  Printer.write(zero);
  delay(100);
  Printer.write(10);
  int i = 0, count = 1; float load = 0;
  for (int y = 0; y < x; y++) {
    //check for adjusting the print out
    if (count < 10) {
      EEPROM.get(i, load );
      if (load < 10) {
        Printer.print(count); Printer.print("         ");  Printer.print(load, 5); Printer.print("         ");
      }
      else if (load >= 10 && load < 100) {
        Printer.print(count); Printer.print("         ");  Printer.print(load, 5); Printer.print("        ");
      }
      else if (load >= 100) {
        Printer.print(count); Printer.print("         ");  Printer.print(load, 5); Printer.print("       ");
      }

      //Printer.print(count); Printer.print("         "); EEPROM.get(i, load ); Printer.print(load, 5); Printer.print("         ");

      Printer.print(EEPROM.read(i + 4)); Printer.print(":"); Printer.print(EEPROM.read(i + 6)); Printer.print(":"); Printer.print(EEPROM.read(i + 8));
      Printer.println();
    }

    //check for adjusting the print out
    if (count >= 10) {
      EEPROM.get(i, load );
      if (load < 10) {
        Printer.print(count); Printer.print("        ");  Printer.print(load, 5); Printer.print("         ");
      }
      else if (load >= 10 && load < 100) {
        Printer.print(count); Printer.print("        ");  Printer.print(load, 5); Printer.print("        ");
      }
      else if (load >= 100) {
        Printer.print(count); Printer.print("        "); Printer.print(load, 5); Printer.print("       ");
      }

      //Printer.print(count); Printer.print("        "); EEPROM.get(i, load); Printer.print(load, 5); Printer.print("         ");

      Printer.print(EEPROM.read(i + 4)); Printer.print(":"); Printer.print(EEPROM.read(i + 6)); Printer.print(":"); Printer.print(EEPROM.read(i + 8));
      Printer.println();
    }

    i = i + 10; count++;

  }
  // bold text
  Printer.write(27);
  Printer.write(32);
  Printer.write(1);
  delay(100);
  Printer.print("Average: ");    Printer.println(stats.average(math_operationts, x), 5);
  Printer.print("MAX PEAK: ");    Printer.println(stats.maximum(math_operationts, x), 5);
  Printer.print("MIN PEAK: ");    Printer.println(stats.minimum(math_operationts, x), 5);
  Printer.print("Standard Deviation: ");    Printer.println(stats.stdev(math_operationts, x), 5);

  EEPROM.write(samples_num_EEPROM, 0); //for new Batch

}

