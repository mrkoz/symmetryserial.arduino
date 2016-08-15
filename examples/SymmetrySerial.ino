
#include "Arduino.h"
#include "SymmetrySerial.h"

/**************************************************************************************************
** Serial defines & variables
**************************************************************************************************/
#define SERIALBOUDRATE 9600

/**************************************************************************************************
** Bluetooth serial defines & variables
**************************************************************************************************/

#define BTBAUDRATE 9600
#define BTHEARTBEATRATE 10000 //send a helo every 1 second

SymmetrySerial bluetoothComms(Serial1, BTBAUDRATE, BTHEARTBEATRATE, serComProcessMessage, recieveStatusMessage);
#define HELO 0xFB
#define ACK 0xFC
#define NACK 0xFD
#define FAIL 0xFE

#define STATUS_DEBUG_ON 0xF0
#define STATUS_DEBUG_OFF 0xF1
#define STATUS_POWER_UP 0xF2
#define STATUS_POWER_DOWN 0xF3
#define STATUS_RESET_CPU 0xF4
#define STATUS_RESET_COMMS 0xF5
#define STATUS_ERASE_EEPROM 0xF6
#define STATUS_RESET_TO_DEFAULTS 0xF7
#define STATUS_RESET_ZERO_ONE 0xF8
#define STATUS_RESET_ZERO_TWO 0xF9
#define STATUS_AUX 0xFA

//test module - for serial protocol 
#define FEAT_SERIAL_TEST                   0x10
#define FEAT_SERIAL_TEST_SINGLE_DATA       FEAT_SERIAL_TEST + 0x01
#define FEAT_SERIAL_TEST_8_DATA            FEAT_SERIAL_TEST + 0x02
#define FEAT_SERIAL_TEST_64_DATA           FEAT_SERIAL_TEST + 0x03
#define FEAT_SERIAL_TEST_ECHO              FEAT_SERIAL_TEST + 0x04
#define FEAT_SERIAL_TEST_SINGLE_DATA_ECHO  FEAT_SERIAL_TEST + 0x05
#define FEAT_SERIAL_TEST_8_DATA_ECHO       FEAT_SERIAL_TEST + 0x06
#define FEAT_SERIAL_TEST_64_DATA_ECHO      FEAT_SERIAL_TEST + 0x07

#define FEAT_SENSOR_READ_GET             0x20

/*** servo last value ***/
unsigned long lastLoop = millis();

bool debug = true;

int led_state = 0;

/**************************************************************************************************
** runtime, stats and configuration defines, loop, startup and setup methods
**************************************************************************************************/
uint8_t count = 0;
char errorMessage[60];
char tempMessage[60];

//the loop time in MS
int loopTime = 10;
int loopCounter = 0;

void setup(void) {
  pinMode(13, OUTPUT);
  startupSetupSerial();
}

void statusMessageSendCallback(uint8_t message) {
  sprintf(errorMessage, "Status Message sent %d", message);
  echoToSerial(errorMessage);
}

void messageSendCallback() {
  sprintf(errorMessage, "Message sent size %d feature %d", bluetoothComms.messageSend.length, bluetoothComms.messageSend.feature);
  echoToSerial(errorMessage);
}

void loop() {
  bluetoothComms.poll();

  lastLoop = millis();
  count++;

  if (count % 64 == 10) {
    led_state = ! led_state;
    digitalWrite(13, led_state);
  }
  delay(loopTime);
}

/**
 * Start up the serial ports
 */
void startupSetupSerial() {
  Serial.begin(SERIALBOUDRATE);
  bluetoothComms.connect();
}

void debugText(String text) {
    echoToSerial(text);
}

void echoToSerial(String message) {
    Serial.println(message);
}


/**************************************************************************************************
** Module/feature execution section - decisions made based on the last received command
**************************************************************************************************/
void serComProcessMessage() {
    switch (bluetoothComms.messageReceive.feature - (bluetoothComms.messageReceive.feature % 0x10)) {
        case FEAT_SERIAL_TEST:
            commandReceivedModuleTest();
            break;
        case FEAT_SENSOR_READ_GET:
            commandReceivedModuleGet();
            break;
        default:
            sprintf(tempMessage, "uh? %d %d %d", bluetoothComms.messageReceive.length, bluetoothComms.messageReceive.feature, bluetoothComms.messageReceive.feature);
            debugText(tempMessage);
            break;
    }
}

void commandReceivedModuleGet() {
    bluetoothComms.messageSend.length = 20;
    bluetoothComms.messageSend.feature = FEAT_SENSOR_READ_GET;
    for (uint8_t i = 0; i < 20; i++) {
        bluetoothComms.setSendDataAt(i, (analogRead(i)/4));
    }

    bluetoothComms.sendMessage();
}

void commandReceivedModuleTest() {
    switch (bluetoothComms.messageReceive.feature) {
        case FEAT_SERIAL_TEST:
        case FEAT_SERIAL_TEST_SINGLE_DATA:
        case FEAT_SERIAL_TEST_8_DATA:
        case FEAT_SERIAL_TEST_64_DATA:
            featureTestReflectMessage();
            break;
    }
}

void featureTestReflectMessage() {
    bluetoothComms.messageSend.length = bluetoothComms.messageReceive.length;
    bluetoothComms.messageSend.feature = bluetoothComms.messageReceive.feature;
    bluetoothComms.messageSend.checksum = bluetoothComms.messageReceive.checksum;

    for (int i = 0; i < bluetoothComms.messageReceive.length; i++) {
        bluetoothComms.setSendDataAt(i, bluetoothComms.getReceiveDataAt(i));
    }

    bluetoothComms.sendMessage();
}

void recieveStatusMessage(uint8_t message) {
    switch (message) {
        case HELO:
            debugText("HELO received, sending ACK");
            break;
        case ACK:
            debugText("ACK received");
            break;
        case NACK:
            debugText("NACK received");
            break;
        case FAIL:
            debugText("FAIL received");
            break;
        case STATUS_DEBUG_ON:
            debugText("STATUS_DEBUG_ON received");
            break;
        case STATUS_DEBUG_OFF:
            debugText("STATUS_DEBUG_OFF received");
            break;
        case STATUS_POWER_UP:
            debugText("STATUS_POWER_UP received");
            break;
        case STATUS_POWER_DOWN:
            debugText("STATUS_POWER_DOWN received");
            break;
        case STATUS_RESET_CPU:
            debugText("STATUS_RESET_CPU received");
            break;
        case STATUS_RESET_COMMS:
            debugText("STATUS_RESET_COMMS received");
            break;
        case STATUS_ERASE_EEPROM:
            debugText("STATUS_ERASE_EEPROM received");
            break;
        case STATUS_RESET_TO_DEFAULTS:
            debugText("STATUS_RESET_TO_DEFAULTS received");
            break;
        case STATUS_RESET_ZERO_ONE:
            debugText("STATUS_RESET_ZERO_ONE received");
            break;
        case STATUS_RESET_ZERO_TWO:
            debugText("STATUS_RESET_ZERO_TWO received");
            break;
        case STATUS_AUX:
            debugText("STATUS_AUX received");
            break;
    }

}



