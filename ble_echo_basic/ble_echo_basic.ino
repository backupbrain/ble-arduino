#include <CurieBle.h>

static const char* BLUETOOTH_DEVICE_NAME = "EchoServer";

static const char* SERVICE_UUID = "180C"; 
static const char* CHARACTERISTIC_UUID = "2A56";
static const int   TRANSMISSION_LENGTH = 20;

char message[TRANSMISSION_LENGTH];
int messageLength;
const char* uuid;
bool dataWritten = false; 


BLEService service(SERVICE_UUID);
BLECharacteristic characteristic(
  CHARACTERISTIC_UUID,
  BLEWrite | BLERead | BLENotify,
  TRANSMISSION_LENGTH
);

BLEPeripheral blePeripheral;


void onCharacteristicWritten(BLECentral& central, BLECharacteristic &characteristic) {
  dataWritten = true;
  uuid = characteristic.uuid();

  messageLength = characteristic.valueLength();
  memcpy((char*) message, (const char*) characteristic.value(), messageLength);
}

// Client connected.  Print MAC address
void onClientConnected(BLECentral& central) {
  Serial.print("Device connected: ");
  Serial.println(central.address());
}

// Client disconnected
void onClientDisconnected(BLECentral& central) {
  Serial.println("Device disconnected");
}


void sendMessage(unsigned char* message) {
  Serial.print("Sending message: ");
  Serial.println((char*) message);
  characteristic.setValue((const unsigned char*) message, messageLength);
}

void setup() {
  Serial.begin(9600);
  while (!Serial) {;}
  blePeripheral.setLocalName(BLUETOOTH_DEVICE_NAME);

  // attach callback when client connects
  blePeripheral.setEventHandler(
    BLEConnected,
    onClientConnected
  );
  // attach callback when client disconnects
  blePeripheral.setEventHandler(
    BLEDisconnected,
    onClientDisconnected
  );
  
  blePeripheral.setAdvertisedServiceUuid(service.uuid());
  blePeripheral.addAttribute(service);
  blePeripheral.addAttribute(characteristic);

  characteristic.setEventHandler(
    BLEWritten,
    onCharacteristicWritten
  );

  Serial.print("Starting ");
  Serial.println(BLUETOOTH_DEVICE_NAME);
  blePeripheral.begin();
}



void loop() {
  if (dataWritten) {
    dataWritten = false;

    Serial.print("Incoming message found: ");
    Serial.println((char*) message);
    sendMessage((unsigned char*)message);
  }

}
