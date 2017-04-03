#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>

#include "uprot.h"
#include "machinedef.h"

#include <AFMotor.h>
#include <Servo.h>

Enrf24 radio(CSN, CE, IRQ);
const uint8_t rxaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };
const uint8_t txaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x03 };

Servo steerServ;
int angle_us = 1250;
AF_DCMotor engine(2);
int engPower;

void dump_radio_status_to_serialport(uint8_t);

/*============================================================================*/
void setup() {
    /* Hardware setup */
        steerServ.attach(SERVO1,ST_MIN,ST_MAX);
        //steerServ.writeMicroseconds(1250);
        engine.run(RELEASE);
        pinMode(BAT_ADC, INPUT);

    /* Communication initialization */
        //Serial.begin(9600);

        SPI.begin();
        SPI.setDataMode(SPI_MODE0);
        SPI.setBitOrder(MSBFIRST);

    /* nRF24l01+ setup */
        radio.begin();  // Defaults 1Mbps, channel 0, max TX power
        dump_radio_status_to_serialport(radio.radioState());
        radio.setRXaddress((void*)rxaddr);
        radio.setTXaddress((void*)txaddr);
        radio.enableRX();  // Start listening
}

char inbuf[33];
DataCtrl* ptr_cdg;
DataResp response;

void loop() {
    
    /* Listening channel for control */
        int timeout;
        timeout = 50000;
        radio.enableRX();
        while (!radio.available(true)){
            delayMicroseconds(1);
            timeout--;
            if(!timeout){
                timeout = 50000;
                engPower = 0;
                engine.run(RELEASE);
            }
        }
        if (radio.read(inbuf)) {
            // Packet received
            radio.disableRX();
            ptr_cdg = (DataCtrl*)&inbuf;

            // Preparing response
            float bat;
            bat = ((float)(2 * analogRead(BAT_ADC))) * (5.0/1024.0);
            response.battery = bat;
            response.servo_a = angle_us;
            //Serial.println(bat);

            // Sending response
            radio.write(&response,10);
            radio.flush();

            /* Controlling motors and servos */
                int rx_joyx, rx_joyy;

                // Steering
                rx_joyx = -(ptr_cdg->joy_x) / 32;    // 2048/32 = 64
                angle_us += rx_joyx;
                if(angle_us < ST_MIN) angle_us = ST_MIN;
                if(angle_us > ST_MAX) angle_us = ST_MAX;
                steerServ.writeMicroseconds(angle_us);

                // Engine
                rx_joyy = (ptr_cdg->joy_y) / 32;
                engPower = rx_joyy;
                int pwm = abs(engPower)*4;
                if(pwm > 255) pwm = 255;
                if(engPower > 10){
                    engine.run(FORWARD);
                    engine.setSpeed(pwm);
                }else if(engPower < -10){
                    engine.run(BACKWARD);
                    engine.setSpeed(pwm);
                }else{
                    engine.run(RELEASE);
                }
            // debug
/*
            Serial.print("Received packet: ");
            Serial.println(1);

            Serial.print("joy\tx: ");
            Serial.print(ptr_cdg->joy_x);
            Serial.print("\ty: ");
            Serial.print(ptr_cdg->joy_y);
            Serial.print("\tsel: ");
            Serial.print(ptr_cdg->joy_sel);
            
            Serial.print("\tstr: ");
            Serial.print(angle_us);
            Serial.print("\tk_b: ");
            Serial.println(ptr_cdg->key_b);
/*
            Serial.print("acc \tx: ");
            Serial.print((ptr_cdg->acc_x));
            Serial.print("\ty: ");
            Serial.print((ptr_cdg->acc_y));
            Serial.print("\tz: ");
            Serial.println((ptr_cdg->acc_z));*/
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
