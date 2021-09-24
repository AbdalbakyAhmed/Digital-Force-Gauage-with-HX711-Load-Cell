void Thermal_printer_InKg() {
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
      else if ((load / gravity) >= 10) {
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
      else if ((load / gravity) >= 10) {
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
      else if (load >= 10) {
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
      else if (load >= 10) {
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
