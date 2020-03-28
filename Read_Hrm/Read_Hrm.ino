#include <Wire.h>

#include "max30102.h"

const byte oxiInt = 10; // pin connected to MAX30102 INT

const int BUFFER_SIZE = 100;

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
    int i;

    for (i = 0; i < BUFFER_SIZE; i++) {
        Serial.println("Waiting for interrupt");
        
        while(digitalRead(oxiInt) == 1);  //wait until the interrupt pin asserts

        Serial.println("Interrupt cleared");
        
        maxim_max30102_read_fifo((aun_red_buffer+i), (aun_ir_buffer+i));  //read from MAX30102 FIFO
    
        Serial.print(i, DEC);
        Serial.print(F("\t"));
        Serial.print(aun_red_buffer[i], DEC);
        Serial.print(F("\t"));
        Serial.print(aun_ir_buffer[i], DEC);    
        Serial.println("");
    }

    delay(1000);  // Let's not print too fast
}
