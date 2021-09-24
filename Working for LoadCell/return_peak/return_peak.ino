#include "HX711.h"
/////////////////////
#define DOUT  50
#define CLK  51
#define calibration_factor -84864

HX711 scale(DOUT, CLK);

// Calibration magic number for Load Cell
#define touchdetectedpin 11
#define peakdetectedpin 12
#define overloadpin 13
/////////////////////
float Threshold = .2; // to avoid jittters and noise effect and hysterisis effect.
float Touch_Detect = 0.1;
int OverLoad_sample = 330;
////////////////////
const char arr_samples_size = 100;
const char NO_sub_samples = 3;   //if you changed 3 then you must change the equation
float arr_samples_readings[arr_samples_size ];
char reading_counter;
float reading;
float avg_array[arr_samples_size ];
boolean overload = false, peakdone = false, peakdetected = false, touchdetected = false;
float peakvalue;

void setup() {
  pinMode(touchdetectedpin, OUTPUT);
  pinMode(peakdetectedpin, OUTPUT);
  pinMode(overloadpin, OUTPUT);
  Serial.begin(9600);
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
}

void loop() {
  float Xreading = peak_value_found();
  if (Xreading > Touch_Detect) {

    Serial.println("peak  "); Serial.print ((Xreading), 5);Serial.println();
  }
}
float peak_value_found() {
  float exact_peak_value = 0;
  overload = false, peakdone = false, peakdetected = false, touchdetected = false;
  for (reading_counter = 0; reading_counter < arr_samples_size && peakdetected == false; reading_counter++)
  {
    scale.set_scale(calibration_factor);
    float weight = scale.get_units(1);
    reading = weight;
    //Serial.println("My weight is "); Serial.print(weight); Serial.println();
    arr_samples_readings[reading_counter] = reading;
    if ( arr_samples_readings[reading_counter] >= Touch_Detect && touchdetected == false) {
      touchdetected = true; digitalWrite(touchdetectedpin, HIGH);
      //delay(300);
    }
    if (reading_counter >= NO_sub_samples) {
      avg_array[reading_counter] = (arr_samples_readings[reading_counter] + arr_samples_readings[reading_counter - 1] + arr_samples_readings[reading_counter - 2]) / NO_sub_samples;
      if (avg_array[reading_counter] < (avg_array[reading_counter - 1] - Threshold ) && reading > Touch_Detect ) //comparing last two averages stored in samples array
      {
        peakdetected = true;
        digitalWrite(peakdetectedpin, HIGH); digitalWrite(peakdetectedpin, HIGH);
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
  return exact_peak_value;
}
