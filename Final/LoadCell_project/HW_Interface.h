//connection for Keypad

byte rowPins[ROWS] = {26,27,28,29}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {22,23,24,25}; //connect to the column pinouts of the keypad

//connection for LCD
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Calibration magic number for Load Cell
#define touchdetectedpin 52
#define peakdetectedpin 5
#define overloadpin 6 
#define end_test 13

