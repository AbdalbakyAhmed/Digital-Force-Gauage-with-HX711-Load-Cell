/******************************************
	PURPOSE:	Tutorial on storing data in the EEPROM on the ATmega328p
	Created by      Rudy Schlaf for www.makecourse.com
        Inspired by:    http://playground.arduino.cc/Code/EEPROMWriteAnything#.UzhKEdwWX08
	DATE:		3/2014
*******************************************/


#include <avr/eeprom.h>//this is the EEPROM library header from the AVR C++ library. 
                       //The AVR libraries are by default included in the Arduino IDE, and all the methods can be used by including the respective headers.

//A struct is a group of variables that are allocated a block of memory. Since one needs to keep track into which bytes of the EEPROM
//certain variables are stored, the definition of a struct allows to simply retrieve all EEPROM data from a block and then use convenient
//C++ functions to retrieve individual variable values.
struct settings       
{
  float Setting1;
  int Setting2;
};

//now we define three struct objects to save the setting for three users.
settings user1settings;
settings user2settings, user3settings;

int settingsSize = sizeof(settings);//determine the length of the settings objects.

void setup()
{
  Serial.begin(9600);//start serial communication
  
  user1settings.Setting1 = 1.234;//define the "user settings" in the struct objects.
  user1settings.Setting2 = 1234;
  
  user2settings.Setting1 = 2.345;
  user2settings.Setting2 = 2345;
  
  user3settings.Setting1 = 3.456;
  user3settings.Setting2 = 3456;
  
  //write the three struct objects to EEPROM
  eeprom_write_block((const void*)&user1settings, (void*)0, settingsSize);
  eeprom_write_block((const void*)&user2settings, (void*)settingsSize, settingsSize);
  eeprom_write_block((const void*)&user3settings, (void*)(2*settingsSize), settingsSize);
  
  //read the user settings back into the three settings objects. This uses the eeprom_read_block method from the eeprom.h library
  //eeprom_read_block(pointer to the memory block in the SRAM where the struct objects are stored,
  
  // pointer to the first byte of the corresponding block in the EEPROM, length) 
  eeprom_read_block((void*)&user1settings, (void*)0, settingsSize);
  eeprom_read_block((void*)&user2settings, (void*)settingsSize, settingsSize);
  eeprom_read_block((void*)&user3settings, (void*)(2*settingsSize), settingsSize);
  
 //note: (void*)&name generates a pointer variable to the variable 'name'; variable* is a pointer variable
 //      (void*)name 'typecasts' the content of the variable 'name' as a pointer,
 //                i.e. (void*)0 generates a pointer to the zero data element of the memory (or EEPROM in this case).
 //                typecasting means changing a variable from one type into another.
  
  Serial.print("User 1 - Setting 1: "); Serial.println(user1settings.Setting1,5);
  Serial.print("User 1 - Setting 2: "); Serial.println(user1settings.Setting2);
  Serial.print("User 2 - Setting 1: "); Serial.println(user2settings.Setting1,5);
  Serial.print("User 2 - Setting 2: "); Serial.println(user2settings.Setting2);
  Serial.print("User 3 - Setting 1: "); Serial.println(user3settings.Setting1,5);
  Serial.print("User 3 - Setting 2: "); Serial.println(user3settings.Setting2);

}

void loop()
{
//do nothing
}

