#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>

#include "Screen_HX8353E.h"
Screen_HX8353E myScreen;

#include "def.h"

Enrf24 radio(CSN, CE, IRQ);
const uint8_t txaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x02 };
const char *str_on = "CYKA";
const char *str_off = "BLYAT";

void dump_radio_status_to_serialport(uint8_t);

void setup() {
  Serial.begin(9600);

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

   myScreen.begin();

   myScreen.setOrientation(0);
       myScreen.clear(grayColour);
  myScreen.setPenSolid(false);
  myScreen.gText(48, 48, "(FUG)");

  radio.begin();  // Defaults 1Mbps, channel 0, max TX power
  dump_radio_status_to_serialport(radio.radioState());

  radio.setTXaddress((void*)txaddr);
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(str_on);
  myScreen.gText(48, 48, str_on);
  radio.print(str_on);
  radio.flush();  // Force transmit (don't wait for any more data)
  dump_radio_status_to_serialport(radio.radioState());  // Should report IDLE
  delay(1000);

  Serial.print("Sending packet: ");
  Serial.println(str_off);
  myScreen.gText(48, 48, str_off);
  radio.print(str_off);
  radio.flush();  //
  dump_radio_status_to_serialport(radio.radioState());  // Should report IDLE
  delay(1000);
}

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
