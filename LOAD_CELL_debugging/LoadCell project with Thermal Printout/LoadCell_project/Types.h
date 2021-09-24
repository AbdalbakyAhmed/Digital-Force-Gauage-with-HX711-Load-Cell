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
/*
char Keys[ROWS][COLS] = {
  {'1', '2',  '3',      'UP'   },
  {'4', '5',  '6',      'DOWN' },
  {'7', '8',  '9',      'ENTER'},
  {'.', '0',  'PRINT',  'BACK' }
};*/





#define calibration_time 10000 //delay time used in STD Calibration
#define gravity 9.81 //value of gravity
//////////Address of Memory

#define pass_EEPROM 0x0BC2  //password EEPROM address (3010).DEC

#define samples_num_EEPROM 0x0BB8    //num of samples EPPROM address (3000).DEC
#define Overflow_EEPROM 0x0BCC      //OverLoad value EPPROM address  (3020).DEC
#define Touch_detect_EEPROM 0x0BD6 //Touch point EPPROM address      (3030).DEC

#define M_cal_input_EEPROM 0x0BEA     //Manual calib. Ref. value EEPROM address (3050).DEC
#define M_cal_output_EEPROM 0x0C4E    //Manual calib. Measured value EEPROM address (3150).DEC
#define cal_st_EEPROM 0x0CB3          //Manual Calib step position EEPROM address (3251).DEC
#define Manual_calib_cursor_EEPROM 0x0FA0 //Manual Calib array pos EEPROM address (4000).DEC

#define layer5_EEPROM 0x0CE4         //Offst for layer_5  EEPROM address (3300).DEC
#define layer10_EEPROM 0x0CEE        //Offst for layer_10 EEPROM address (3310).DEC
#define layer15_EEPROM 0x0CF8        //Offst for layer_15 EEPROM address (3320).DEC
#define layer20_EEPROM
#define layer25_EEPROM
#define layer30_EEPROM

