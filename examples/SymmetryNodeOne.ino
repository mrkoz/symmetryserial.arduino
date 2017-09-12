/* includes */
  #include <SymmetrySerial.h>         // My balanced protocol

/* defines & globals */
  #define HELO          0xFB
  #define ACK           0xFC
  #define NACK          0xFD
  #define FAIL          0xFE

  #define FEATURE_ONE   0x10
  #define MESSAGE_GO    0x11
  #define MESSAGE_STOP  0x12

  #define FEATURE_TWO   0x20
  #define MESSAGE_DRAW  0x21
  #define MESSAGE_PAINT 0x22

  #define FEATURE_DATA        0x20
  #define MESSAGE_DATA_SINGLE 0x21
  #define MESSAGE_DATA_LONG   0x22

  SymmetrySerial bluetoothComms(&Serial3, 9600, 2000);
  unsigned long lastLoop = millis();

/* setup */
  void setup(void) {  
    Serial.begin(9600); 
    bluetoothComms.setCallBacks(ProcessComMessage, recieveStatusMessage);
    bluetoothComms.connect();
  }

/* main loop */
  void loop() {
    // Any messages for me?
    bluetoothComms.poll();
  }

/* serial functions */
  void ProcessComMessage() {
    switch (bluetoothComms.getReceiveFeatureSet()) {
      case FEATURE_ONE:
        featureOneCalled();
        break;
      case FEATURE_TWO:
        featureTwoCalled();
        break;
      default:
        break;
    }
  }

  void recieveStatusMessage(uint8_t message) {
    switch (message) {
      case HELO:
        if (debug) {
          Serial.println("HELO, sending ACK");
        }
        bluetoothComms.sendStatusACK();
        break;
      case ACK:
        if (debug) {
          Serial.println("ACK received");
        }
        break;
      case NACK:
        if (debug) {
          Serial.println("NACK received");
        }
        break;
      case FAIL:
        if (debug) {
          Serial.println("FAIL received");
        }
        break;
    }
  }

/* basic functions */
  void sendMaybe() {

    if (millis() % 10000 < 1000) {
      bluetoothComms.sendMessageSingle(FEATURE_ONE);
      bluetoothComms.sendMessage();
    }
    if (millis() % 10000 > 3000 && millis() % 10000 < 4000 ) {
      bluetoothComms.sendMessageSingle(FEATURE_ONE);
      bluetoothComms.sendMessage();
    }
    if (millis() % 10000 > 6000 && millis() % 10000 < 7000 ) {
      bluetoothComms.sendMessageSingle(FEATURE_ONE);
      bluetoothComms.sendMessage();
    }
    if (millis() % 10000 > 9000) {
      bluetoothComms.sendMessageSingle(FEATURE_ONE);
      bluetoothComms.sendMessage();
    }
  }

  }
  void featureOneCalled() {
    switch (bluetoothComms.getReceiveFeature()) {
        case MESSAGE_GO:
          Serial.println("Got a message! - feature one, GO")
          break;
        case MESSAGE_STOP:
          Serial.println("Got a message! - feature one, STOP")
          break;
    }
  }

  void featureTwoCalled() {
    switch (bluetoothComms.getReceiveFeature()) {
        case MESSAGE_DRAW:
          Serial.println("Got a message! - feature two, DRAW")
          break;
        case MESSAGE_PAINT:
          Serial.println("Got a message! - feature two, PAINT")
          break;
    }
  }