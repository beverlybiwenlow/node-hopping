#include <SPI.h>
#include <RH_RF95.h>

/* for feather m0 RFM9x*/
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

struct dataStruct{
  int weight; 
  int pulseNum;
}myData, rxData;
byte tx_buf[sizeof(myData)] = {0};

int pulseCount = 0;
double previousMillis;
boolean toTransmit = false;

void setup(){
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  while(!Serial){
    delay(1);
  }
  delay(100);

  Serial.println("Feather LoRa TXRX Test");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while(!rf95.init()){
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)){
    Serial.println("setFrequency failed");
    while(1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
  
  // Initialize data. Weight fixed but packetnum changes.
  myData.weight = 2;
  myData.packetnum = 0;

  // Initialize interrupt
  attachInterrupt(5, interrupt_func, CHANGE);
}


void loop(){
  delay(550);
  
  if(toTransmit){
    if(myData.packetnum != 0){
      
      // Transmit data
      Serial.print("Transmitting..."); // Send a message to rf95_server
      byte data_size = sizeof(myData);
      memcpy(tx_buf, &myData, data_size);
      Serial.print(myData.weight);
      Serial.println(myData.packetnum);
      SPI.usingInterrupt(5);
      rf95.send((uint8_t *)tx_buf, data_size);
      rf95.waitPacketSent();
      rf95.setModeRx();
      toTransmit = false;
    }
  }
  
  else{
   if (rf95.available()){
    // For the message received
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)){
      rf95.printBuffer("Received: ", buf, len);
      memcpy(&rxData, buf, sizeof(rxData));
      Serial.print("Got: ");
      Serial.print(rxData.weight);
      Serial.println(rxData.packetnum);
//      Serial.print("RSSI: ");
//      Serial.println(rf95.lastRssi(), DEC);

      // Send received data onwards
      if(rxData.weight <= myData.weight){
        rf95.send(buf, sizeof(buf));
        rf95.waitPacketSent();
        Serial.println("Sent data on"); 
      }     
    }
    else{
      Serial.println("Receive failed");   
    }
  }

  // If there is no message to receive, do nothing
  else{
  }
} 
  }

void interrupt_func(){
  double currentMillis = millis();
  if (currentMillis - previousMillis > 3000){

    // Reset cycle
    Serial.println("New cycle");
    myData.packetnum = pulseCount;
    pulseCount = 0;
    toTransmit = true;
  }

  // Regardless of new cycle or nots
  previousMillis = currentMillis;
  pulseCount++;
  Serial.println(pulseCount);
}

