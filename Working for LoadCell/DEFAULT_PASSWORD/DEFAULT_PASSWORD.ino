#include<EEPROM.h>
#define pass_EEPROM 0x0BC2
void setup() {
  // put your setup code here, to run once:
for(int j=0;j<4;j++)
  EEPROM.write(pass_EEPROM+j, j+49);//(j+49)as we deal with the ASCII values, if want '1', 49 represents that
}

void loop() {
  // put your main code here, to run repeatedly:

}
