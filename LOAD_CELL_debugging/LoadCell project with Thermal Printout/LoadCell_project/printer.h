#include "Arduino.h"
#include <SoftwareSerial.h>

/*
 * The circuit:
 RX is digital pin 2 (connect to TX of other device)
 TX is digital pin 3 (connect to RX of other device)
 * Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69
*/

SoftwareSerial Printer(2,3); //(Rx,Tx)
#define heatTime 80
#define heatInterval 255
#define printDensity 15 
#define printBreakTime 15
int zero=0;

void initPrinter()
{
 //Modify the print speed and heat
 Printer.write(27);
 Printer.write(55);
 Printer.write(7); //Default 64 dots = 8*('7'+1)
 Printer.write(heatTime); //Default 80 or 800us
 Printer.write(heatInterval); //Default 2 or 20us
 //Modify the print density and timeout
 Printer.write(18);
 Printer.write(35);
 int printSetting = (printDensity<<4) | printBreakTime;
 Printer.write(printSetting); 
}



