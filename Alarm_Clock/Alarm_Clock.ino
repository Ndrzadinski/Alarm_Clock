// Nathan Drzadinski
// 11/18/2020
//Alarm Clock

#include <dht.h>


#define dht_apin A1 // Analog Pin sensor is connected to

dht DHT;


//Set up for Humidity Sensor Button
const int humidButt = 2;


//Set Up Clock Buttons
const int holdtoAlarm = 13;
const int holdtoEdit = 7;
const int pushtoEditSec = 3;
const int pushtoEditHour = 8;

//Global Set Up for bit shift operator (digit pattern)
// pin 11 of 74HC595 (SHCP)
const int bit_clock_pin = 5;
// pin 12 of 74HC595 (STCP)
const int digit_clock_pin = 6;
// pin 14 of 74HC595 (DS)
const int data_pin = 4;
// digit pattern for a 7-segment display
const byte digit_pattern[11] =
{
  B00111111,  // 0
  B00000110,  // 1
  B01011011,  // 2
  B01001111,  // 3
  B01100110,  // 4
  B01101101,  // 5
  B01111101,  // 6
  B00000111,  // 7
  B01111111,  // 8
  B01101111,  // 9
 /* B01110111,  // A
  B01111100,  // b
  B00111001,  // C
  B01011110,  // d
  B01111001,  // E
  */
  B01110001   // F
};

//Set Up for for which digit
const int rrDigit = 12;
const int rmDigit = 11;
const int lmDigit = 10;
const int llDigit = 9;

//Set arrays
int dataArray[4]={0,0,0,0};
int alarmArray[4]={0,0,0,0};
int digitArray[4]={1,2,3,4};
int humidArray[3]={0,0,10};

//Variable for interrupt and time
int timer=0;
int sec;

void setup()
{
  pinMode(data_pin, OUTPUT);
  pinMode(bit_clock_pin, OUTPUT);
  pinMode(digit_clock_pin, OUTPUT);  

  pinMode(rrDigit, OUTPUT);
  pinMode(rmDigit, OUTPUT);
  pinMode(lmDigit, OUTPUT);
  pinMode(llDigit, OUTPUT);
//If the button is pressed then thay are HIGH
  pinMode(humidButt, INPUT);
  pinMode(holdtoEdit, INPUT);
  pinMode(pushtoEditHour, INPUT);
  pinMode(holdtoAlarm, INPUT);
  pinMode(pushtoEditSec, INPUT);

  //ISR timer set up
  TCCR0A=(1<<WGM01);    //Set the CTC mode   
  OCR0A=0xF9; //Value for ORC0A for 1ms 
  
  TIMSK0|=(1<<OCIE0A);   //Set the interrupt request
  sei(); //Enable interrupt
  
  TCCR0B|=(1<<CS01);    //Set the prescale 1/64 clock
  TCCR0B|=(1<<CS00);
}
 
void update_one_digit(int data, int digit)
{
  byte pattern;
  
  // get the digit pattern to be updated
  pattern = digit_pattern[data];

  // turn off the output of 74HC595
  digitalWrite(digit_clock_pin, LOW);
  
  // update data pattern to be outputed from 74HC595
  shiftOut(data_pin, bit_clock_pin, MSBFIRST, pattern);

  //Change Digit
  if (digit==1){
    digitalWrite(rrDigit, LOW);
    digitalWrite(rmDigit, HIGH);
    digitalWrite(lmDigit, HIGH);
    digitalWrite(llDigit, HIGH);
  }
  else if (digit==2){
    digitalWrite(rrDigit, HIGH);
    digitalWrite(rmDigit, LOW);
    digitalWrite(lmDigit, HIGH);
    digitalWrite(llDigit, HIGH);
  }
  else if (digit==3){
    digitalWrite(rrDigit, HIGH);
    digitalWrite(rmDigit, HIGH);
    digitalWrite(lmDigit, LOW);
    digitalWrite(llDigit, HIGH);
  }
  else{
    digitalWrite(rrDigit, HIGH);
    digitalWrite(rmDigit, HIGH);
    digitalWrite(lmDigit, HIGH);
    digitalWrite(llDigit, LOW);
  }
  
  // turn on the output of 74HC595
  digitalWrite(digit_clock_pin, HIGH);
}

void loop()
{ 
  if (timer>=1000){
    sec++;
    timer=0;
  }
  else if (sec>=60){
    dataArray[0]++;
    if (dataArray[0]>9){
      dataArray[0]=0;
      dataArray[1]++;
      if (dataArray[1]>=6){
        dataArray[1]=0;
        dataArray[2]++;
        if (dataArray[2]>9){
          dataArray[2]=0;
          dataArray[3]++;
        }
      }
    }
    if (dataArray[3]>=2 && dataArray[2]>3){
      dataArray[2]=0;
      dataArray[3]=0;
    }
    sec=0;
  }
  else if (digitalRead(humidButt)==HIGH){
    DHT.read11(dht_apin);
    double tempf=(DHT.temperature)*9/5+32;
    int unit=10;
    int ones=(int)(tempf-((int)(tempf/10))*10);
    int tens=(int)(tempf/10);
    while(digitalRead(humidButt)==HIGH){
      update_one_digit(unit,1);
      delay(1);
      update_one_digit(ones,2);
      delay(1);
      update_one_digit(tens,3);
      delay(1);
    }
  }
  //Set Time
  else if (digitalRead(holdtoEdit)==HIGH){
    for (int i=0;i<4;i++){
      update_one_digit(dataArray[i],digitArray[i] );
      delay(2);
    }
    if (digitalRead(pushtoEditSec)==HIGH){
      dataArray[0]++;
      if (dataArray[0]>9){
        dataArray[0]=0;
        dataArray[1]++;
        if (dataArray[1]>=6){
          dataArray[1]=0;
        }
        while(digitalRead(pushtoEditSec)==HIGH){
          for (int i=3;i>=0;i--){
            update_one_digit(dataArray[i],digitArray[i]);
            delay(2);
          }
        }
      }
      while(digitalRead(pushtoEditSec)==HIGH){
          for (int i=3;i>=0;i--){
            update_one_digit(dataArray[i],digitArray[i]);
            delay(2);
          }
        }
    }
    if (digitalRead(pushtoEditHour)==HIGH){
      dataArray[2]++;
      if (dataArray[2]>9){
        dataArray[2]=0;
        dataArray[3]++;
        while(digitalRead(pushtoEditHour)==HIGH){
          for (int i=3;i>=0;i--){
            update_one_digit(dataArray[i],digitArray[i]);
            delay(2);
          }
        }
      }
      if (dataArray[3]>=2 && dataArray[2]>3){
        dataArray[2]=0;
        dataArray[3]=0;
        while(digitalRead(pushtoEditHour)==HIGH){
          for (int i=3;i>=0;i--){
            update_one_digit(dataArray[i],digitArray[i]);
            delay(2);
          }
        }
      }
      while(digitalRead(pushtoEditHour)==HIGH){
        for (int i=3;i>=0;i--){
          update_one_digit(dataArray[i],digitArray[i]);
          delay(2);
        }
      }
    }
  }
  //Set Alarm
  else if (digitalRead(holdtoAlarm)==HIGH){
    for (int i=0;i<4;i++){
      update_one_digit(alarmArray[i],digitArray[i] );
      delay(2);
    }
    if (digitalRead(pushtoEditSec)==HIGH){
      alarmArray[0]++;
      if (alarmArray[0]>9){
        alarmArray[0]=0;
        alarmArray[1]++;
        if (alarmArray[1]>=6){
          alarmArray[1]=0;
        }
      }
      while(digitalRead(pushtoEditSec)==HIGH){
        for (int i=3;i>=0;i--){
          update_one_digit(alarmArray[i],digitArray[i]);
          delay(2);
        }
      }
    }
    if (digitalRead(pushtoEditHour)==HIGH){
      alarmArray[2]++;
      if (alarmArray[2]>9){
        alarmArray[2]=0;
        alarmArray[3]++;
      }
      if (alarmArray[3]>=2 && alarmArray[2]>3){
          alarmArray[3]=0;
          alarmArray[2]=0;
      }
      while(digitalRead(pushtoEditHour)==HIGH){
        for (int i=3;i>=0;i--){
          update_one_digit(alarmArray[i],digitArray[i]);
          delay(2);
        }
      }
    }
  }
  //Always Display Time
  else {
    // get the value to be displayed and update one digit
    for (int i=3;i>=0;i--){
      update_one_digit(dataArray[i],digitArray[i]);
      delay(2);
    }
  }
}

//Interrupt
ISR(TIMER0_COMPA_vect){    //This is the interrupt request
  timer++;
}
