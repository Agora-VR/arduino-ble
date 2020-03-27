
#include <ArduinoBLE.h>

const int buttonPin = 2;
const int ledPin =  LED_BUILTIN;

const char serviceUuid[] = "8bff20de-32fb-4350-bddb-afe103ef9640";
const char characteristicUuid[] = "1c8dd778-e8c3-45b0-a9f3-48c33a400315";

int buttonState = 0;

BLEService buttonService(serviceUuid);
BLEBoolCharacteristic buttonStatus(characteristicUuid, BLERead | BLENotify);

void setup() {
  // Start serial connection for printing
  Serial.begin(9600);
  while (!Serial);
  
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }
  
  BLE.setLocalName("BLEButton");
  BLE.setAdvertisedService(buttonService);
  buttonService.addCharacteristic(buttonStatus);
  BLE.addService(buttonService);
  
  BLE.advertise();

  Serial.println("Starting!");
}

void loop() {
  BLEDevice central = BLE.central();
  
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) {
      buttonState = digitalRead(buttonPin);

      // Activate LED if button is pressed
      if (buttonState == HIGH) {
        digitalWrite(ledPin, HIGH);
      } else {
        digitalWrite(ledPin, LOW);
      }

      if (buttonStatus.value() != buttonState) {
        buttonStatus.writeValue(buttonState);
        Serial.println("Updating value!");
      }
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
