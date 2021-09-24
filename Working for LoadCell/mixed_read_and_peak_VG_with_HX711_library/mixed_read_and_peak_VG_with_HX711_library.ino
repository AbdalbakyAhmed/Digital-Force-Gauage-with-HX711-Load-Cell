#include "Arduino.h"
#include "HX711.h"

#define RST 4
#define DOUT  3
#define CLK  2


// Create our LoadCell object
HX711 scale(DOUT, CLK);

// Calibration magic number for Load Cell
double calibration_factor =-84864/9.81 ;//2680.00/31.58;
unsigned long time;
const int touchdetectedpin=7;
 const int peakdetectedpin=12;
 const int overloadpin=13;
 long peak_drop_threshold=.2;// to avoid jittters and noise effect and hysterisis effect.
 long touchlimit=0.5;
 long overloadlimit=330;
const int NO_of_all_readings =100;
const int NO_sub_samples=3;//if you changed 3 then you must change the equation
double all_samples_readings_array[NO_of_all_readings];
int reading_counter;
double reading;
double avg_array[NO_of_all_readings];
boolean overload=false,peakdone=false,peakdetected=false,touchdetected=false;
double peakvalue;


//////////////////////////

void setup() 
{
  
      
       pinMode(touchdetectedpin,OUTPUT);
       
       pinMode(peakdetectedpin,OUTPUT);
       
       pinMode(overloadpin,OUTPUT);
       
        ///////////////////////////
        Serial.begin(9600);
  Serial.println("calibrated load cell sketche");
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  // Reset pin (to re-zero the weight not reset the board!)
  pinMode(RST, INPUT_PULLUP);
}
 
 /////////////////////////////////////////

void loop(){
  for (reading_counter=0;reading_counter<NO_of_all_readings&&peakdetected==false;reading_counter++)
  {
    scale.set_scale(calibration_factor);
    // Read an average of X readings between brackets
    //Serial.println("Reading weight");
    double weight = scale.get_units(1);
    reading= weight;
    all_samples_readings_array[reading_counter]=reading;           
    Serial.print ("all_samples_readings_array[reading_counter] = \t");
    Serial.print (all_samples_readings_array[reading_counter]); 
    Serial.print ("\t N \t");//storing sensor readings in variable called reading stores its value in readingsarray with readingscounter
    Serial.print (" \t at reading_counter =\t ");
    Serial.println (reading_counter);
    
    if ( all_samples_readings_array[reading_counter]>=touchlimit && touchdetected==false){
      touchdetected=true; digitalWrite(touchdetectedpin,HIGH);
      Serial.println ("touchdetected \t  ready to break samples \t ");delay(1000);}
    if (all_samples_readings_array[reading_counter]>=overloadlimit){
      overload=true; digitalWrite(overloadpin,HIGH);
      Serial.println ("overload detected");
      delay(10000);}           
    //starting to have shifted average over NO of sub samples after taking readings of NO of sub samples_samples.
    if (reading_counter>=NO_sub_samples){
      //avg_array[reading_counter]=all_samples_readings_array[reading_counter];
      avg_array[reading_counter]=(all_samples_readings_array[reading_counter]+all_samples_readings_array[reading_counter-1]+all_samples_readings_array[reading_counter-2])/NO_sub_samples;
      Serial.print ("avg_array[reading_counter]= \t");
      Serial.println (avg_array[reading_counter]);
      //comparsion part between two successuve moving averages 
      if (avg_array[reading_counter]< (avg_array[reading_counter-1]-peak_drop_threshold ) &&reading>touchlimit )//comparing last two averages stored in samples array
      {
        Serial.print("avg_array[reading_counter]=\t ");
        Serial.println(avg_array[reading_counter]);
        peakdetected=true;
        digitalWrite(peakdetectedpin,HIGH);digitalWrite(peakdetectedpin,HIGH);
        Serial.println("peak detected because of new average is less than older ");
        Serial.print(avg_array[reading_counter]);
        Serial.print(" \t is less than \t");
        Serial.print(avg_array[reading_counter-1] );
        Serial.print("\t at reading_counter=\t");
        Serial.println(reading_counter);
        Serial.print(" peak value average is = \t ");
        peakvalue=all_samples_readings_array[reading_counter-1];
        Serial.println(peakvalue);
                        ////////////////////////////////////////////////////////////////////////////////////////////////////
                       //last all main samples subreadings maximum detection part-NO_checked_samples for exact peak value
                      ///////////////////////////////////////////////////////////////////////////////////////////////////
        if (peakdetected==true&&peakdone==false){ 
          Serial.println ("checking maxumum value part " );                       
          int  k=reading_counter; 
          double exact_peak_value = all_samples_readings_array[k];
          for (int i = reading_counter; i >=0 ;i--){     
            Serial.print("all_samples_readings_array[i] = \t ");
            Serial.print(i);
            Serial.print("\t");
            Serial.println(all_samples_readings_array[i]);
            if (all_samples_readings_array[i] > exact_peak_value)//exact_peak_value is temporarly storage of temporal peak value till finishing check process.
            {
              exact_peak_value = all_samples_readings_array[i];
              k = i;double  exact_peak_value= all_samples_readings_array[k];}
            }
          Serial.print ("exact peak value is   =  \t " );
          Serial.print (exact_peak_value);
          delay(25000);
          peakdone=true;
          break;
          //end bracket of maximum finding      
          } 
      }         
    } 
  }
}   
   
