// Core library for code-sense
#if defined(ENERGIA) // LaunchPad MSP430, Stellaris and Tiva, Experimeter Board FR5739 specific
#include "Energia.h"
#else // error
#error Platform not defined
#endif

#define CSN 36
#define CE 35
#define IRQ_NRF 41

// Prototypes

// Include application, user and local libraries
#include "SPI.h"

#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>

Enrf24 radio(CE, CSN, IRQ_NRF);  // P2.0=CE, P2.1=CSN, P2.2=IRQ
const uint8_t txaddr[] = {0,0,0,0,1};
const char *str_on = "ON";
const char *str_off = "OFF";
//void dump_radio_status_to_serialport(uint8_t);

#include "Screen_HX8353E.h"
//Screen_HX8353E myScreen;

// Define variables and constants
#define joystickX 2
#define joystickY 26
uint16_t x, y, x00, y00;
uint16_t colour;
uint32_t z;

// Add setup code
void setup()
{


    //myScreen.begin();
    x00 = 0;
    y00 = 0;

    // MSP432 14-bit set to 12-bit
#if defined(__MSP432P401R__)
    analogReadResolution(12);
#endif

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  radio.begin();  // Defaults 1Mbps, channel 0, max TX power
  radio.setTXpower(-18);
  radio.setTXaddress((void*)txaddr);
}

// Add loop code
void loop()
{
    x = map(analogRead(joystickX), 0, 4096, 0, 128);
    y = map(analogRead(joystickY), 0, 4096, 128, 0);
    if (x < 1)      x = 1;
    if (x > 126)    x = 126;
    if (y < 1)      y = 1;
    if (y > 126)    y = 126;
    /*
    if ((x00 != x) || (y00 != y)) {
        z = (uint32_t)((x-64)*(x-64) + (y-64)*(y-64)) >> 8;
        if (z > 4)      colour = redColour;
        else if (z > 1) colour = yellowColour;
        else            colour = greenColour;
        
        myScreen.dRectangle(x00-1, y00-1, 3, 3, blackColour);
        myScreen.dRectangle(x-1, y-1, 3, 3, colour);
        x00 = x;
        y00 = y;
    }
    
    
    myScreen.gText(0, myScreen.screenSizeY()-myScreen.fontSizeY(),
                   "x=" + i32toa((int16_t)x-64, 10, 1, 6) + " y=" + i32toa(64-(int16_t)y, 10, 1, 6),
                   colour);

                   */
    radio.print(str_on);
    radio.flush();  // Force transmit (don't wait for any more data)
    delay(500);
  
    radio.print(str_off);
    radio.flush();  
    delay(500);
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



