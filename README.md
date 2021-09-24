## Digital Force Gauage with `HX711 Load Cell`

**This project was aimed to monitor maximum load 300 N (may be more) or max 30.6 Kg.F by using 50 Kg capacity load cell**

- Interfacing mentioned load cell with 2mv/V output with Arduino through HX711 taking readings with highest possible rate 
  - “EX: every 1 millisecond” >> displaying reading on LCD screen in N ,or KG.F with 0.1N resolution at least (.001 is recommended)

- Updating the quick readings from digital output signal according to value of reading as follows:
  - Reading increases from zero to just 0.1N (adjustable value under protected engineering level) >> Activate `digital output1` `touching signal`.
  - Exceeding limit of load cell designed capacity (330 N) (adjustable value under protected engineering level)  >> Activate `(overload)` `digital output2` and `alarm message` on screen “overload “.
  - Peak value of successive continuous readings  >> Activate `(peak value)` `digital output3`
  - Messaging of hardness = "the reading of local peak ,or maximum load value"

- Digital input `end of test` from external board:
  - if system counts one peak of only Single sample, mode will be displayed on screen and just having peak value which is ready on screen and ready to be printed with print button from keypad.
  - If system read N of  samples, the `automatic batch mode` will be displayed on screen and will ask user about batch ID includes 12 digit Batch Number entry.  
  - And internal counter will count till reaching end of batch at N samples storing all samples values on memory preparing it for printing and statistics.
 
- Printing samples readings, or peak values on normal printer :
  - its readings basic statistics { average ,min, maximum, standard deviation }  
  - Enter and Print Date and Time to the report. (real time and date).
  - Space to put manufacturer data ,and  other to customer data.
  - Enter a 12 digit Batch Number label appears on display and in printed report.
  - Calibration report at basic  three points 5, 10, and 15 Kg.F. for load cell raw readings , and another ten points for any arbitrary readings point in piece-wise linear way.
  - Print-out of force increase curve VS. time for samples.
  - Display: LCD Display for No. of samples, and hardness, or peak value results.

System interface connections:
