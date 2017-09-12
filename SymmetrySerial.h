/*
  SymmetrySerial.h - Library for SymmetrySimple code.
  Created by Adam Purdie, August 2, 2016.
  Licence as per Licence.txt
*/
#ifndef SymmetrySerial_h
#define SymmetrySerial_h

/* Defines */

#define TIMEOUT_SERIAL_BUFFER 1000
//serial message = [command/size][option1][option2][...data...][check]
#define SERIAL_BUFFER_SIZE 64 //Arduino serial length is 64
#define SERIAL_MESSAGE_SIZE 3
#define SERIAL_MESSAGE_BUFFER_SIZE SERIAL_BUFFER_SIZE - SERIAL_MESSAGE_SIZE - 1 // takign into account the start bit
#define S_SIZE_OR_STATUS 0
#define S_FEATURE 1
#define S_CHECKSUM 2
#define MSG_START 0xFF

/* status commands for basic */
#define HELO 0xFB
#define ACK 0xFC
#define NACK 0xFD
#define FAIL 0xFE

//TODO: make the send/receive buffers feature to be able to handle multiple messages in a queue
//TODO: if we're doing the above we might as well add message numbers!? this is getting too hard.
//TODO: make a separated i2c version that has a destination and SLAVE/MASTER preset?

#define MESSAGE_DATA_BUFFER_BLANK 0xFF

/* Structs */
typedef struct {
  uint8_t    length;
  uint8_t    feature;
  uint8_t    checksum;
  uint8_t    dataBuffer[SERIAL_MESSAGE_BUFFER_SIZE];
} serialMessage;

#include "Arduino.h"

class SymmetrySerial
{
  public:
    /***** Instance Members *****/
    serialMessage    messageReceive;
    serialMessage    messageSend;

    uint16_t failedMessageCount = 0;
    uint16_t successMessageCount = 0;

    bool debug = false;
    bool configured = true;

    unsigned long lastHeartbeat = 5000;
    unsigned long lastMessage = 0;
    unsigned long lastCycle = 0;
    bool heartbeatDead = false;

    uint8_t counterSend = 0;
    uint8_t counterReceive = 0;

    /***** Public functions *****/
    /* Constructor */
    SymmetrySerial(HardwareSerial *port, int baudRate);
    /* Constructor with heartbeat */
    SymmetrySerial(HardwareSerial *port, int baudRate, unsigned long heartBeat);
    /* set callbacks */
    void setCallBacks(void (*callback)(void), void (*statusCallback)(uint8_t message));
    
    /***** Port connect/disconnect *****/
    /* Stop serial port connectivity */
    void connect();
    /* Stop serial port connectivity */
    void disconnect();
    /* poll to see if there's new data and process while available */
    void poll();
    /* set the message length */
    void sendSetMessageLength(uint8_t length);
    /* work's out the value based on the contents - dangerous if there are values = MESSAGE_DATA_BUFFER_BLANK (0xFF) */
    void sendSetMessageLengthAuto();
    /* react to status message */
    void statusMessageReceived(uint8_t message);
    /* send a status message */
    void sendSatusMessage(uint8_t command);
    /* pre-canned status messages - HELO */
    void sendStatusHELO();
    /* pre-canned status messages - ACK */
    void sendStatusACK();
    /* pre-canned status messages - NACK */
    void sendStatusNACK();
    /* pre-canned status messages - FAIL */
    void sendStatusFAIL();
    /* send a message */
    void sendMessage();
    /* quick send for single feature trigger and value */
    void sendMessageSingle(uint8_t feature, uint8_t value);
    /* quick send for single feature trigger */
    void sendMessageSingle(uint8_t feature);
    /* set the feature type for the send packet */
    void setSendFeature(uint8_t feature);
    /* get the feature set type from the received packet */
    uint8_t getReceiveFeatureSet();
    /* get the feature type from the received packet */
    uint8_t getReceiveFeature();
    /* get the data length of the received packet */
    uint8_t getReceiveLength();
    /* get the checksum of the received packet */
    uint8_t getReceiveChecksum();
    /* get the value of the receive dataset at position */
    uint8_t getReceiveDataAt(uint8_t position);
    /* set the value of the receive dataset at position to value */
    void setReceiveDataAt(uint8_t position, uint8_t value);
    /* get the value of the send dataset at position */
    uint8_t getSendDataAt(uint8_t position);
    /* set the value of the send dataset at position to value */
    void setSendDataAt(uint8_t position, uint8_t value);

    /* receive buffer purge */
    void purgeMessageReceive();
    /* send buffer purge */
    void purgeMessageSend();

    /* Add data helpers for send packet */
    void addByteToSend(uint8_t data);
    void addWordToSend(uint16_t data);
    void resetSendDataCounter();
    void setSendDataCounterTo(uint8_t value);

    /* Get data from receive packet */
    uint8_t getByteFromReceive();
    uint16_t getWordFromReceive();
    void resetReceiveDataCounter();
    void setReceiveDataCounterTo(uint8_t value);

    bool is_alive();

  private:
    /***** Private Members *****/
    HardwareSerial * _port;

    unsigned long _baudRate;
    unsigned long _heartBeat;

    uint8_t receiving = 0;
    uint8_t receiveCount = 0;
    uint8_t recChar = 0x00;

    uint8_t receiveChecksum = 0x00;

    void (*_messageCallback)(void);
    void (*_statusCallback)(uint8_t message);
    void (*_messageSendCallback)(void);
    void (*_messageStatusSendCallback)(uint8_t message);

    /***** Private functions *****/
    /* updates last message and last heartbeat on data received */
    void dataReceived();
    /* checks the heartbeat timeout and fires off a HELO if needed */
    void checkheartBeat();
    /* checksum calculation - receive */
    uint8_t getRecieveBufferChecksum();
    /* checksum calculation - send */
    uint8_t getSendBufferChecksum();

};

#endif
