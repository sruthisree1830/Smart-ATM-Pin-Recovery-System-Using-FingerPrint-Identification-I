#include <SPI.h>
#include <RFID.h>
#define SDA_DIO 5
#define RESET_DIO 22
#define buz 4

RFID RC522(SDA_DIO, RESET_DIO); 

void setup()
{ 
  Serial.begin(9600);
  SPI.begin(); 
  RC522.init();
  pinMode(buz , OUTPUT);
}

void loop()
{
getCardId();
}
void getCardId(){
  if (RC522.isCard())
  {
    RC522.readCardSerial();
    digitalWrite(buz,HIGH);
    delay(100);
    String id = "CardId :";
    for(int i=0;i<5;i++)
    {
    if(RC522.serNum[i]==0){
      continue;
    }
    id = id + String(RC522.serNum[i],HEX);
    }
    Serial.print(id);
    Serial.println(",");

  }
  digitalWrite(buz,LOW); 
}
