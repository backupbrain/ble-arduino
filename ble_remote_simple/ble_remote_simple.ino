#include <CurieBle.h>

static const char* BLUETOOTH_DEVICE_NAME = "LedRemote";

static const char* SERVICE_UUID = "180C"; 
static const char* CHARACTERISTIC_UUID = "2A56";
static const int   TRANSMISSION_LENGTH = 2; 
static const unsigned int FOOTER_POSITION = TRANSMISSION_LENGTH - 1;
static const unsigned int DATA_POSITION = 0;

static const unsigned int COMMAND_LED_OFF = 0x01; 
static const unsigned int COMMAND_LED_ON = 0x02;

static const unsigned int MESSAGE_TYPE_ERROR = 0x00;
static const unsigned int MESSAGE_TYPE_CONFIRMATION = 0x01;
static const unsigned int MESSAGE_TYPE_COMMAND = 0x02;


static const byte ledPin = 13;
static const unsigned int LED_STATE_ERROR = 0x00;
static const unsigned int LED_STATE_ON = 0x01;
static const unsigned int LED_STATE_OFF = 0x02;
int ledState = LED_STATE_OFF;

char message[TRANSMISSION_LENGTH];
int messageLength;
const char* uuid;
bool commandReceived = false;


BLEService service(SERVICE_UUID);
BLECharacteristic characteristic(
  CHARACTERISTIC_UUID,
  BLEWrite | BLERead | BLENotify,
  TRANSMISSION_LENGTH
);

BLEPeripheral blePeripheral;


void onCharacteristicWritten(BLECentral& central, 
  BLECharacteristic &characteristic) {
    
  commandReceived = true;
  uuid = characteristic.uuid();

  messageLength = characteristic.valueLength();
  memcpy((char*) message, (const char*) characteristic.value(), messageLength);
}

void sendCommandConfirmation() {
  byte confirmation[TRANSMISSION_LENGTH] = {0x0};
  confirmation[DATA_POSITION] = (byte)ledState;
  confirmation[FOOTER_POSITION] = (byte)MESSAGE_TYPE_CONFIRMATION;
  characteristic.setValue((const unsigned char*) confirmation, TRANSMISSION_LENGTH);
}

void turnLedOn() {
  Serial.println("Turning LED on");
  digitalWrite(ledPin, HIGH);
  ledState = LED_STATE_ON;
}
void turnLedOff() {
  Serial.println("Turning LED off");
  digitalWrite(ledPin, LOW);
  ledState = LED_STATE_OFF;
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


void setup() {
  Serial.begin(9600);
  while (!Serial) {;}

  turnLedOff();
  
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
  if (commandReceived) {
    commandReceived = false;
  
    // incoming command is one byte
    unsigned int command = message[DATA_POSITION]; 
    if (command == COMMAND_LED_ON) {
      turnLedOn();
      sendCommandConfirmation();
    } else {
      turnLedOff();
      sendCommandConfirmation();
    }
  }

}
