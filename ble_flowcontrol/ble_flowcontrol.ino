#include "CurieBle.h"

static const char* bluetoothDeviceName = "MyDevice"; 

static const int   characteristicTransmissionLength = 20; 
static const char* bleFlowControlMessage = "ready";
static const int   bleFlowControlMessageLength = 5;

// store details about transmission here
struct BleTransmission {
  char* data;
  unsigned int length;
  const char* uuid;
};
BleTransmission bleTransmissionData;
bool bleDataWritten = false; // true if data has been received

BLEService service("1800"); 
BLECharacteristic characteristic(
  "2803",
  BLERead | BLENotify | BLEWrite, // read, write, notify
  characteristicTransmissionLength
);

BLEPeripheral blePeripheral; 

bool bleCharacteristicSubscribed = false; // true when a client subscribes


// When data is sent from the client, it is processed here inside a callback
// it is best to handle the result of this inside the main loop
void onCharacteristicWritten(BLECentral& central, BLECharacteristic &characteristic) {
  bleDataWritten = true; // alert that data has been written
  
  bleTransmissionData.uuid = characteristic.uuid();
  bleTransmissionData.length = characteristic.valueLength();
  `
  // Since we are playing with strings, use strcpy
  strcpy(bleTransmissionData.data, (char*) characteristic.value());
}

void setCharacteristicValue(char* output, int length) {
  characteristic.setValue((const unsigned char*) output, length); 
}

void setup() {
  blePeripheral.setLocalName(bluetoothDeviceName); 

  blePeripheral.setAdvertisedServiceUuid(service.uuid()); 
  blePeripheral.addAttribute(service);
  blePeripheral.addAttribute(characteristic);

  blePeripheral.begin(); 
}

void loop() {
  
  // if the bleDataWritten flag has been set, print out the incoming data
  if (bleDataWritten) {
    bleDataWritten = false;
    
    Serial.print(bleTransmissionData.length);
    Serial.print(" bytes sent to characteristic ");
    Serial.print(bleTransmissionData.uuid);
    Serial.print(": ");
    Serial.println(bleTransmissionData.data);
    
    // send out flow control message
    setCharacteristicValue((char*) bleFlowControlMessage, bleFlowControlMessageLength);
  }
  
}

