// -----------------------------------------------------------------------------
// Wireless Cooking Thermometer - App For Decoding Tapped Receiver Output Signal
// -----------------------------------------------------------------------------
//
// --- Temperature Data - Message Protocol/Layout ------------------------------
// Single transmission reception on the receiver obtains 4*8 bytes long message
// i.e. inclusive of 8 redundancies. Such burst happens every 5 seconds. Here's
// a sample message without redundancy: 01000001 00001001 10111110 11110110,
// which is equal to 109 Fahrenheit.
//
// Here's an apporach on how to decode the above 4 bytes message packet:
// ---------------------------------------------------------
// |      |  P1  |  P2  |  P3  |                           |
// ---------------------------------------------------------
// | N1   |  N2  |  N3  |  N4  |  NN1 |  NN2 |  NN3 |  NN4 |
// ---------------------------------------------------------
// | 0100 | 0001 | 0000 | 1001 | 1011 | 1110 | 1111 | 0110 | <- 109 Fahrenheit
// ---------------------------------------------------------
// N1 (Nibble 1) = Transmitter identifier of half a byte. Note! If the
//                 transmitter is restarted then this nibble will differ.
// N2 (Nibble 2/P1) = Payload >> 1st digit of temperature data in Fahrenheit
// N3 (Nibble 3/P2) = Payload >> 2nd digit of temperature data in Fahrenheit
// N4 (Nibble 4/P3) = Payload >> 3rd digit of temperature data in Fahrenheit
// NN1..NN4 (Negated Nibbles) = These nibbles are redundant but negated/complement/bitwise-NOT
//                              of N1, N2, N3, N4 nibbles respectively.
//
// --- Hardware devices used in this project -----------------------------------
// - Biltema wireless cooking thermometer transmitter/receiver >>
//    http://www.biltema.fi/BiltemaDocuments/Manuals/84-0867_man.pdf
// - Arduino UNO hardware clone "DCcduino R3 Uno"
//
// --- Helpful Sources ---------------------------------------------------------
// A. Hardware hacking
//  - http://www.gregorystrike.com/2011/03/13/wireless-thermometer-hacking-for-homebrew-beer/
// B. Wireless temperature sensor signal decoding
//  - https://forums.adafruit.com/viewtopic.php?f=8&t=25414&sid=54c464374c3d81593cb93bce74476251&start=15#p322178
//  - http://rayshobby.net/reverse-engineer-wireless-temperature-humidity-rain-sensors-part-1/
// C. Miscellaneous
//  - RF protocol description >> http://wmrx00.sourceforge.net/Arduino/OregonScientific-RF-Protocols.pdf
//  - N.. number of wireless sensors decoding approaches >> https://github.com/merbanan/rtl_433
//  - HowTo: Capture RF-Link b/t thermometer Sender and Receiver >> http://wiki.openpicus.com/index.php?title=Wifi_bbq
//  - HowTo: Decode wireless signal broadcast from Maverick ET-732 BBQ thermometer >> https://hackaday.io/post/15045
//  - Signal decoding with Arduino
//    i. Arduino as a logic analyser >> https://github.com/gillham/logic_analyzer
//    ii. Logic analyser GUI >> http://ols.lxtreme.nl/
//    iii. Intro to Arduino as a logic analyser >> https://www.youtube.com/watch?v=vQ5TvnRdHMM
// -----------------------------------------------------------------------------

// ----------
// CONSTANTS
// ----------
// --- Pins --------------------------------------------------------------------
int pinData = 3;                // ORANGE wire connected to 3rd pin, which
                                // provides temperature data every 5 seconds
int pinLED_ReceivingData = 13;  // LED for indicating data reception on the
                                // receiver end
// --- Pulse Interval ----------------------------------------------------------
int lowLogicPulseInterval = 2500;   // 2.5µs (microseconds)
int highLogicPulseInterval = 5000;  // 5µs (microseconds)
//int addedBit = 15;                // Other bits present after the redundant
                                    // messages: 15
//int pulsesInTotal = 256+addedBit; // Total num of High-Low logic signals with
                                    // 8 redundancies: 256
int addedBit = 1;                   // Bit value present after the message: 1
int pulsesInTotal = 32+addedBit;    // Total num of High-Low logic signals
                                    // without redundancy: 32
// --- Data Packet -------------------------------------------------------------
int nibbleBitCount = 4;
int transmitterId_NibbleCount = 1;
int payload_NibbleCount = 3;

// ----------
// VARIABLES
// ----------
unsigned int i = 0;
unsigned int x = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(pinData, INPUT);
  pinMode(pinLED_ReceivingData, OUTPUT);
}

void loop()
{
  // ToDo: React to transmitter being turned-off

  // ToDo: React to receiver being turned-off

  if (digitalRead(pinData) == LOW) {
    digitalWrite(pinLED_ReceivingData, HIGH); // Flag arrival of data
    String dataPacket = convertPluseToBinaryDataPacket();
    if (!isInvalidData(dataPacket)) {
      //Serial.println("Temperature:");
      Serial.println(getPayload(dataPacket));
      delay(1500);
    }
    else {
      Serial.println("ERROR: Invalid Data");
    }
  }
  else {
    digitalWrite(pinLED_ReceivingData, LOW);
  }
}

// --------------
// LOCAL METHODS
// --------------
String convertPluseToBinaryDataPacket() {
  i = 0;
  int rawData[pulsesInTotal];
  String dataPacket = "";
  do {
    rawData[i] = pulseIn(pinData, HIGH);
    i = i + 1;
    if (i == pulsesInTotal) {
      // --- Signal Protocol Decoding ---
      for (x = 0; x < i; x++) {
        switch (decodePulseTimeInterval(rawData[x])) {
          case 0:
            dataPacket = dataPacket + "0";
            break;
          case 1:
            dataPacket = dataPacket + "1";
            break;
          default:
            dataPacket = dataPacket + "\n";
            break;
        }
      }
      //Serial.println("\n--- Raw binary packets with possible redundancies ---");
      //Serial.println(dataPacket);
    }
  } while (i < pulsesInTotal);
  return dataPacket;
}

int decodePulseTimeInterval(int time) {
  // Pulse Time-Interval determines if it is a long or short logic
  // Note! Pulse Time-Interval is in µs (microseconds)
  if (time > 0 && time < highLogicPulseInterval) {
    if (time < lowLogicPulseInterval) { return 0; }
    else { return 1; }
  }
  // Pulse is 0 or greater, means a High-Logic-Pulse-Interval
  else { return -1; }
}

boolean isInvalidData(String dataPacket) {
  // ------------------------------
  // Case1: Detect data corruption
  // ------------------------------
  // Subcase1: On incorrect record length
  // Note! This case also tackles such a condition:
  //  * Nibbles generated when any of the buttons are  being pressed on the
  //    receiver device
  if (dataPacket.length() != pulsesInTotal) {
    Serial.println("Fault: Pulse Length");
    return true;
  }
  // Subcase2: Verify & validate negation
  else {
    //--- TransmitterId nibbles ---
    if (isInvalidNegation(getTransmitterId(dataPacket),
          getNegatedTransmitterId(dataPacket))) {
      Serial.println("Fault: InvalidNegation TransmitterId");
      return true;
    }
    //--- Payload nibbles ---
    if (isInvalidNegation(getPayload(dataPacket),
          getNegatedPayload(dataPacket))) {
      Serial.println("Fault: InvalidNegation Payload");
      return true;
    }
    return false;
  }

  // ToDo: Case2: Detect tranmitter temperature sensor is detatched
  // ---------------------------------------------------------------
  // 0111 1010 1010 1010 1000 0101 0101 0101
  // ---------------------------------------------------------------
}

boolean isInvalidNegation(String nibbles, String negatedNibbles) {
  // --- Check bits for negation correctness ---
  for(x=0; x<nibbles.length(); x++) {
    if (nibbles[x] == '0') {
      if(negatedNibbles[x] == '0') {
        return true;
      }
    }
    else if (nibbles[x] == '1') {
      if(negatedNibbles[x] == '1') {
        return true;
      }
    }
  }
  return false;
}

String getPayload(String dataPacket) {
  return dataPacket.substring((transmitterId_NibbleCount * nibbleBitCount) + 1,
    (transmitterId_NibbleCount * nibbleBitCount) +
    (payload_NibbleCount * nibbleBitCount) + 1);
}

String getNegatedPayload(String dataPacket) {
  return dataPacket.substring((nibbleBitCount*5) + 1,
    (nibbleBitCount*5) + (nibbleBitCount*payload_NibbleCount) + 1);
}

String getTransmitterId(String dataPacket) {
  return dataPacket.substring(1, (transmitterId_NibbleCount * nibbleBitCount) + 1);
}

String getNegatedTransmitterId(String dataPacket) {
  return dataPacket.substring((nibbleBitCount*4) + 1,
    (nibbleBitCount*4) + (transmitterId_NibbleCount * nibbleBitCount) + 1);
}
