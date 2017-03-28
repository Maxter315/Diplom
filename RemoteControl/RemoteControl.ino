#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>
#include <stdio.h>

#include "Screen_HX8353E.h"

#include "uprot.h"
#include "def.h"

Screen_HX8353E mkScreen;
Enrf24 radio(CSN, CE, IRQ);
const uint8_t txaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };
const uint8_t rxaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x03 };
DataCtrl codogr;

void dump_radio_status_to_serialport(uint8_t);
void spiSwitch(uint8_t dev);

/*============================================================================*/
void setup() {
    /* EDU_MK2 Joystick setup */
        pinMode(JOY_SEL, INPUT_PULLUP);
        analogReadResolution(12);
        pinMode(KEY_A, INPUT_PULLUP);
        pinMode(KEY_B, INPUT_PULLUP);

    /* Communication initialization */
        Serial.begin(9600);

        SPI.begin();
        SPI.setDataMode(SPI_MODE0);
        SPI.setBitOrder(MSBFIRST);

    /* EDU_MK2 LCD setup */
        mkScreen.begin();
        mkScreen.setOrientation(0);
        mkScreen.clear(blackColour);
        mkScreen.setPenSolid(false);
        mkScreen.gText(48, 48, "(FUG)");

    /* nRF24l01+ setup */
        radio.begin();  // Defaults 1Mbps, channel 0, max TX power
        dump_radio_status_to_serialport(radio.radioState());
        radio.setTXaddress((void*)txaddr);
        radio.setRXaddress((void*)rxaddr);
}

int16_t xx, yy;
int16_t ax, ay, az;
float mainBattery;

char inbuf[33];
DataResp* ptr_cdg;

void loop() { 

    /* Reading Joystick position and orientation */
        xx = analogRead(JOY_X) - 2048;
        yy = analogRead(JOY_Y) - 2048;

        ax = analogRead(ACC_X) - 2048;
        ay = analogRead(ACC_Y) - 2048;
        az = analogRead(ACC_Z) - 2048;

        // Forming data to transmitt 
        codogr.setJoy(xx, yy, !(bool)digitalRead(JOY_SEL));
        codogr.setKeys(!(bool)digitalRead(KEY_A), !(bool)digitalRead(KEY_B));
        codogr.setAcc(ax, ay, az);

    /* Switching SPI to LCD */
        spiSwitch(2);

        // Displaying all the information
        char val[10];
        //mkScreen.rectangle(0,0,128,32,blackColour);
        sprintf(val,"jx=%6.1d",xx);    mkScreen.gText(0, 0, val);
        sprintf(val,"jy=%6.1d",yy);    mkScreen.gText(0, 8, val);
        sprintf(val,"ax=%6.1d",ax);    mkScreen.gText(64, 0, val);
        sprintf(val,"ay=%6.1d",ay);    mkScreen.gText(64, 8, val);
        sprintf(val,"az=%6.1d",az);    mkScreen.gText(64, 16, val);
        mkScreen.gText(0, 16, " ");

    /* Switching SPI to nRF24l01+ */
        spiSwitch(1);

        // Sending packet to the Machine
        radio.write(&codogr,16);
        radio.flush();
        radio.enableRX();
        
        // Awainting for response from the Machine
        int timeout;
        timeout = 5000;
        while (!radio.available(true) && timeout){
            delayMicroseconds(1);
            timeout--;
        }
        if(radio.read(inbuf)){
            // Response is received
            ptr_cdg = (DataResp*)&inbuf;
            Serial.print("bat: ");
            Serial.println(ptr_cdg->battery);
        }
        radio.disableRX();
        
        // debug
        Serial.print("acc \tx: ");
        Serial.print(ax);
        Serial.print("\ty: ");
        Serial.print(ay);
        Serial.print("\tz: ");
        Serial.println(az);

    delay(500);
}
/*============================================================================*/

void spiSwitch(uint8_t dev){
    if(dev == 1){
        //nRF24
        digitalWrite(LCD_CS,HIGH);    pinMode(LCD_CS,OUTPUT);
        digitalWrite(CSN,LOW);       pinMode(CSN,OUTPUT);
    }else if(dev == 2){
        digitalWrite(CSN,HIGH);       pinMode(CSN,OUTPUT);
        digitalWrite(LCD_CS,LOW);    pinMode(LCD_CS,OUTPUT);
    }
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
