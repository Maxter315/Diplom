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
int steerPos;
AF_DCMotor engine(4);
int engPower;

void dump_radio_status_to_serialport(uint8_t);

/*============================================================================*/
void setup() {
    /* Hardware setup */
        steerServ.attach(SERVO1,600,2400);
        steerServ.write(90);
        engine.run(RELEASE);

    /* Communication initialization */
        Serial.begin(9600);

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
                motor.run(RELEASE);
            }
        }
        if (radio.read(inbuf)) {
            // Packet received
            radio.disableRX();
            ptr_cdg = (DataCtrl*)&inbuf;

            // Preparing response
            float bat;
            bat = 2 * analogRead(BAT_ADC) * (5.0/1024.0);
            response.battery = bat;
            response.servo_a = steerServ;

            // Sending response
            radio.write(&response,8);
            radio.flush();

            /* Controlling motors and servos */
                steerPos += ptr_cdg->joy_x;
                engPower = ptr_cdg->joy_y;
                if(engPower > 10){
                    motor.run(FORWARD);
                    motor.setSpeed(map(abs(engPower),0,2048,0,255));
                }else if(engPower > -10){
                    motor.run(BACKWARD);
                    motor.setSpeed(map(abs(engPower),0,2048,0,255));
                }else{
                    motor.run(RELEASE);
                }
                
            // debug
            Serial.print("Received packet: ");
            Serial.println(1);

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
