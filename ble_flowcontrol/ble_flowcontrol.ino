#include <CurieBle.h>

static const char* bluetoothDeviceName = "MyDevice"; 

static const char* serviceUUID = "1800";
static const char* characteristicUUID = "2803";
static const int   transmissionLength = 20; 
static const char* flowControlMessage = "ready";
static const int   flowControlMessageLength = 5;

// store details about transmission here
struct transmission {
  char* data;
  unsigned int length;
  const char* uuid;
};
transmission transmissionData;
bool dataWritten = false; // true if data has been received

BLEService service(serviceUUID); 
BLECharacteristic characteristic(
  characteristicUUID,
  BLERead | BLENotify | BLEWrite, // read, write, notify
  transmissionLength
);

BLEPeripheral blePeripheral; 

bool characteristicSubscribed = false; // true when a client subscribes


// When data is sent from the client, it is processed here inside a callback
// it is best to handle the result of this inside the main loop
void onCharacteristicWritten(BLECentral& central, BLECharacteristic &characteristic) {
  dataWritten = true;
  
  transmissionData.uuid = characteristic.uuid();
  transmissionData.length = characteristic.valueLength();

  const unsigned int length = characteristic.valueLength();

  // Since we are playing with strings, we need to make sure the string terminates
  char data[length+1];
  memcpy(data, characteristic.value(), (length)*sizeof(char));
  data[length] = '\0';
  
  transmissionData.data = data;
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
  
  // if the dataWritten flag has been set, print out the incoming data
  if (dataWritten) {
    dataWritten = false;
    
    Serial.print(transmissionData.length);
    Serial.print(" bytes sent to characteristic ");
    Serial.print(transmissionData.uuid);
    Serial.print(": ");
    Serial.println(transmissionData.data);
    
    // send out flow control message
    setCharacteristicValue((char*) flowControlMessage, flowControlMessageLength);
  }
  
}

