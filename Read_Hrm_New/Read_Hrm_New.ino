#include <Wire.h>

#include "algorithm_by_RF.h"
#include "max30102.h"

const byte oxiInt = 10; // pin connected to MAX30102 INT

uint8_t uch_dummy;

uint32_t aun_ir_buffer[BUFFER_SIZE]; //infrared LED sensor data
uint32_t aun_red_buffer[BUFFER_SIZE];  //red LED sensor data


void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(oxiInt, INPUT);

  Wire.begin();

  maxim_max30102_reset(); //resets the MAX30102
  delay(1000);

  maxim_max30102_read_reg(REG_INTR_STATUS_1,&uch_dummy);  //Reads/clears the interrupt status register
  maxim_max30102_init();  //initialize the MAX30102
}

void loop() {
  float n_spo2,ratio,correl;  //SPO2 value
  int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
  int32_t n_heart_rate; //heart rate value
  int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid
  
  int i;
  
  for (i = 0; i < BUFFER_SIZE; i++) {
    while(digitalRead(oxiInt) == 1);  //wait until the interrupt pin asserts
    
    maxim_max30102_read_fifo((aun_red_buffer+i), (aun_ir_buffer+i));  //read from MAX30102 FIFO

//    Serial.print(i, DEC);
//    Serial.print(F("\t"));
//    Serial.print(aun_red_buffer[i], DEC);
//    Serial.print(F("\t"));
//    Serial.print(aun_ir_buffer[i], DEC);
//    Serial.println("");
  }

  rf_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid, &ratio, &correl); 

  Serial.println("--RF--");
//  Serial.print(elapsedTime);
//  Serial.print("\t");
  Serial.print(n_spo2);
  Serial.print("\t");
//  Serial.print(n_heart_rate, DEC);
  Serial.println(n_heart_rate, DEC);
//  Serial.print("\t");
//  Serial.println(hr_str);
  Serial.println("------");
  
  delay(1000);  // Let's not print too fast
}
