#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CSN 10
#define CE 9

RF24 radio(CE, CSN);

//const byte rxAddr[6] = "00001";
const uint8_t rxAddr[] = {0,0,0,0,1};
void setup()
{
  while (!Serial);
  Serial.begin(9600);

  Serial.println("machine test");
  
  radio.begin();
  radio.openReadingPipe(0, rxAddr);
  
  radio.startListening();
}

void loop()
{
  if (radio.available())
  {
    Serial.print("x:");
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    
    Serial.println(text);
  }
}
