#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>

#include "uprot.h"
#include "machinedef.h"

#include <Servo.h>

Enrf24 radio(CSN, CE, IRQ);
const uint8_t rxaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };
const uint8_t txaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x03 };

void dump_radio_status_to_serialport(uint8_t);

/*============================================================================*/
void setup() {
  Serial.begin(9600);

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  
  radio.begin();  // Defaults 1Mbps, channel 0, max TX power
  dump_radio_status_to_serialport(radio.radioState());

  radio.setRXaddress((void*)rxaddr);
  radio.setTXaddress((void*)txaddr);
  
  radio.enableRX();  // Start listening
}


void loop() {
    char inbuf[33];
    DataCtrl* ptr_cdg;
    DataResp response; 

radio.enableRX();
    while (!radio.available(true));
    if (radio.read(inbuf)) {
    response.battery = 120;
    radio.disableRX();
    
    radio.write(&response,8);
    radio.flush();

    Serial.print("Received packet: ");
    Serial.println(1);

    ptr_cdg = (DataCtrl*)&inbuf;
    Serial.print("joy\tx: ");
    Serial.print(ptr_cdg->joy_x);
    Serial.print("\ty: ");
    Serial.print(ptr_cdg->joy_y);
    Serial.print("\tsel: ");
    Serial.print(ptr_cdg->joy_sel);
    
    Serial.print("\tk_a: ");
    Serial.print(ptr_cdg->key_a);
    Serial.print("\tk_b: ");
    Serial.println(ptr_cdg->key_b);

    Serial.print("acc \tx: ");
    Serial.print((ptr_cdg->acc_x));
    Serial.print("\ty: ");
    Serial.print((ptr_cdg->acc_y));
    Serial.print("\tz: ");
    Serial.println((ptr_cdg->acc_z));
  }
}

/*============================================================================*/

void dump_radio_status_to_serialport(uint8_t status)
{
  Serial.print("Enrf24 radio transceiver status: ");
  switch (status) {
    case ENRF24_STATE_NOTPRESENT:
      Serial.println("NO TRANSCEIVER PRESENT");
      break;

    case ENRF24_STATE_DEEPSLEEP:
      Serial.println("DEEP SLEEP <1uA power consumption");
      break;

    case ENRF24_STATE_IDLE:
      Serial.println("IDLE module powered up w/ oscillators running");
      break;

    case ENRF24_STATE_PTX:
      Serial.println("Actively Transmitting");
      break;

    case ENRF24_STATE_PRX:
      Serial.println("Receive Mode");
      break;

    default:
      Serial.println("UNKNOWN STATUS CODE");
  }
}
