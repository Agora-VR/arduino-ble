
#include <ArduinoBLE.h>
#include <Wire.h>

#include "algorithm_by_RF.h"
#include "max30102.h"

const char serviceUuid[] = "8bff20de-32fb-4350-bddb-afe103ef9640";
const char characteristicUuid[] = "1c8dd778-e8c3-45b0-a9f3-48c33a400315";
const char oxygenationCharacteristicUuid[] = "b8ae0c39-6204-407c-aa43-43087ec29a63";

BLEService hrmService(serviceUuid);
BLEUnsignedIntCharacteristic heartRateReading(characteristicUuid, BLERead | BLENotify);
BLEFloatCharacteristic oxygenationReading(oxygenationCharacteristicUuid, BLERead | BLENotify);

const byte oxiInt = 10; // pin connected to MAX30102 INT

uint8_t uch_dummy;

uint32_t aun_ir_buffer[BUFFER_SIZE]; //infrared LED sensor data
uint32_t aun_red_buffer[BUFFER_SIZE];  //red LED sensor data


void setup() {
  Wire.begin();

  maxim_max30102_reset(); //resets the MAX30102
  delay(1000);

  maxim_max30102_read_reg(REG_INTR_STATUS_1,&uch_dummy);  //Reads/clears the interrupt status register
  maxim_max30102_init();  //initialize the MAX30102

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("AgoraVR-BLE");
  BLE.setAdvertisedService(hrmService);
  hrmService.addCharacteristic(heartRateReading);
  hrmService.addCharacteristic(oxygenationReading);
  BLE.addService(hrmService);

  BLE.advertise();

  Serial.println("Starting!");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      float n_spo2,ratio,correl;  //SPO2 value
      int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
      int32_t n_heart_rate; //heart rate value
      int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid

      int i;

      for (i = 0; i < BUFFER_SIZE; i++) {
        while(digitalRead(oxiInt) == 1);  //wait until the interrupt pin asserts

        maxim_max30102_read_fifo((aun_red_buffer+i), (aun_ir_buffer+i));  //read from MAX30102 FIFO
      }

      rf_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid, &ratio, &correl);

      heartRateReading.writeValue(n_heart_rate);
      oxygenationReading.writeValue(n_spo2);

      Serial.println("--RF--");
      Serial.print(n_spo2);
      Serial.print("\t");
      Serial.println(n_heart_rate, DEC);
      Serial.println("------");

      delay(1000);  // Let's not print too fast
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
