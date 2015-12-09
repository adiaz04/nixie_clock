#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

int led = 13;
//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // wait for serial
  delay(200);
  Serial.println("DS1307RTC Read Test");
  Serial.println("-------------------");
  pinMode (led, OUTPUT);
  // Shift Register Pins.
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  digitalWrite(led, !digitalRead(led));
  tmElements_t tm;
  
  if (RTC.read(tm)) {
    printTime(&tm);
    updateDisplay(&tm);
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}


// Prints the current time to the serial port.
void printTime(tmElements_t* tm) {
 Serial.print("Ok, Time = ");
    print2digits(tm->Hour);
    Serial.write(':');
    print2digits(tm->Minute);
    Serial.write(':');
    print2digits(tm->Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm->Day);
    Serial.write('/');
    Serial.print(tm->Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm->Year));
    Serial.println(); 
}

void updateDisplay (tmElements_t* tm) {
  // currentTime{hour, hour, min, min, sec, sec}
  int currentTime [] = {0,0,0,0,0,0};
  int binaryTime[3];
  currentTime [0] = tm->Hour / 10;
  currentTime [1] = tm->Hour % 10;
  
  currentTime [2] = tm->Minute / 10;
  currentTime [3] = tm->Minute % 10;
  
  currentTime [4] = tm->Second / 10;
  currentTime [5] = tm->Second % 10;

  convertToBinaryTime (currentTime, binaryTime);

   // Shift register test
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite (latchPin, LOW);
  // shift out the bits:
  
  for (int i = 0; i < 3; ++i)
  {
    shiftOut (dataPin, clockPin, MSBFIRST, binaryTime[i]);
  }  
  //take the latch pin high so the LEDs will light up:
  digitalWrite (latchPin, HIGH); 
}

// Converts the current time to an arry of 3 integers, that will be decoded 
//into a 6 digit time by the Binary to Decimal decoders. 
void convertToBinaryTime (int currentTime[], int *binaryTime) {
   int hourFirstDigit = (currentTime [0] << 4);
   int hourSecondDigit = currentTime [1];
   binaryTime [0] = (hourFirstDigit | hourSecondDigit);
   
   int minuteFirstDigit = (currentTime [2] << 4);
   int minuteSecondDigit = currentTime [3];
   binaryTime [1] = (minuteFirstDigit | minuteSecondDigit);
   
   int secondFirstDigit = (currentTime [4] << 4);
   int secondSecondDigit = currentTime [5];
   binaryTime [2] = (minuteFirstDigit | minuteSecondDigit);
}
