#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>
#include <stdio.h>

#include "Screen_HX8353E.h"
Screen_HX8353E myScreen;

#include "uprot.h"
#include "def.h"

Enrf24 radio(CSN, CE, IRQ);
const uint8_t txaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };
const uint8_t rxaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x03 };
const char *str_on = "CYKA";
const char *str_off = "BLYAT";

DataCtrl codogr;

void dump_radio_status_to_serialport(uint8_t);
void spiSwitch(uint8_t dev);

/*=========================================================================*/
void setup() {
    pinMode(JOY_SEL, INPUT_PULLUP);
    analogReadResolution(12);

    pinMode(KEY_A,INPUT_PULLUP);
    pinMode(KEY_B,INPUT_PULLUP);

  Serial.begin(9600);

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  myScreen.begin();
  myScreen.setOrientation(0);
  myScreen.clear(blackColour);
  myScreen.setPenSolid(false);
  myScreen.gText(48, 48, "(FUG)");

  radio.begin();  // Defaults 1Mbps, channel 0, max TX power
  dump_radio_status_to_serialport(radio.radioState());

  radio.setTXaddress((void*)txaddr);
  radio.setRXaddress((void*)rxaddr);
  
}

int16_t xx, yy;
int16_t ax, ay, az;
        char inbuf[33];
        DataResp* ptr_cdg;

void loop() { 

    spiSwitch(2);
    
    xx = analogRead(JOY_X) - 2048;
    yy = analogRead(JOY_Y) - 2048;
    
    ax = analogRead(ACC_X) - 2048;
    ay = analogRead(ACC_Y) - 2048;
    az = analogRead(ACC_Z) - 2048;
    
    codogr.setJoy(xx, yy, !(bool)digitalRead(JOY_SEL));
    codogr.setKeys(!(bool)digitalRead(KEY_A), !(bool)digitalRead(KEY_B));
    codogr.setAcc(ax, ay, az);

    char val[10];

    //myScreen.rectangle(0,0,128,32,blackColour);
    sprintf(val,"jx=%6.1d",xx);    myScreen.gText(0, 0, val);
    sprintf(val,"jy=%6.1d",yy);    myScreen.gText(0, 8, val);
    sprintf(val,"ax=%6.1d",ax);    myScreen.gText(64, 0, val);
    sprintf(val,"ay=%6.1d",ay);    myScreen.gText(64, 8, val);
    sprintf(val,"az=%6.1d",az);    myScreen.gText(64, 16, val);
    myScreen.gText(0, 16, " ");

    spiSwitch(1);
  //radio.print(str_off);
    radio.write(&codogr,16);
    radio.flush();  //
    radio.enableRX();
        int cnt;

        cnt = 5000;
        
        while (!radio.available(false) && cnt) {delayMicroseconds(1); cnt--;}
        if(radio.read(inbuf)){
            ptr_cdg = (DataResp*)&inbuf;
            Serial.print("bat: ");
            Serial.println(ptr_cdg->battery);
        }
        radio.disableRX();
        
    Serial.print("acc \tx: ");
    Serial.print(ax);
    Serial.print("\ty: ");
    Serial.print(ay);
    Serial.print("\tz: ");
    Serial.println(az);
    delay(1000);
}
/*=========================================================================*/
void spiSwitch(uint8_t dev){
    if(dev == 1){
        //nRF24
        //delay(1);
        digitalWrite(LCD_CS,HIGH);    pinMode(LCD_CS,OUTPUT);
        //delay(1);
        digitalWrite(CSN,LOW);       pinMode(CSN,OUTPUT);
        //delay(1);    
    }else if(dev == 2){
        //delay(1);
        digitalWrite(CSN,HIGH);       pinMode(CSN,OUTPUT);
        //delay(1);
        digitalWrite(LCD_CS,LOW);    pinMode(LCD_CS,OUTPUT);
        //delay(1);
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
