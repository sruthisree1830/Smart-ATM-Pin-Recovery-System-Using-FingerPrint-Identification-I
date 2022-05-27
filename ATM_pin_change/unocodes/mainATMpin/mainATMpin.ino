
#include <HardwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <SPI.h>
#include <MFRC522.h>



/////////////////////////////////////////////////////////////////////////////////////


#define BSerial Serial2
#define buz 4
#define RST_PIN         22
#define SS_PIN          5
#define but             21

HardwareSerial HSerial1(1);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&BSerial);

MFRC522 mfrc522(SS_PIN, RST_PIN);

uint8_t id;

void setup() {
    HSerial1.begin(9600, SERIAL_8N1, 12, 13);
    Serial.begin(9600);
    finger.begin(57600);
    delay(5);
      if (finger.verifyPassword()) {
    ;
  } else {
    while (1) { delay(1); }
  }
  SPI.begin(); 
  mfrc522.PCD_Init();  
  pinMode(buz,OUTPUT);
  digitalWrite(buz,LOW);
  pinMode(but, INPUT);
  
}

/////////////////////////////////////////////////////////////////////////////////////

void loop() {
  String data = "";
  if(Serial.available()){
    data = Serial.readString();
    if(data.indexOf("getcardid") != -1)getCardId();
    if(data.indexOf("getfingerindex") != -1)get_index();
    if(data.indexOf("otp:") != -1){
      int otp = data.substring(data.indexOf("otp:")+4,data.indexOf(',')).toInt();
      String num = data.substring(data.indexOf("mobile:")+7,data.indexOf(';'));
      String mobile = "91"+String(num);
      data = "HELLO\n\n Your OTP for Pin change is :"+String(otp);
      send_sms(mobile,data);
      Serial.print(otp);
      Serial.print(" : ");
      Serial.print(mobile);
      Serial.print(" , ");
      Serial.println("SMS sent");
    }
    
    //HSerial1.println(data);
  }

  if(digitalRead(but) == 0){
    Serial.println("Tap Your Card :");
    getCardId();
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    id = readnumber();
    if (id == 0) {// ID #0 not allowed, try again!
     return;
    }
    Serial.print("Enrolling ID #");
    Serial.println(id);

    while (!  getFingerprintEnroll() );
    }
  delay(50); 

  
}

/////////////////////////////////////////////////////////////////////////////////////


void get_index(){
while(1){
  if(getFingerprintID())break;
}
delay(50);
}

/////////////////////////////////////////////////////////////////////////////////////



bool getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER:
      return false;
    case FINGERPRINT_PACKETRECIEVEERR:
      return false;
    case FINGERPRINT_IMAGEFAIL:
      return false;
    default:
      return false;
  }
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return false;
    case FINGERPRINT_PACKETRECIEVEERR:
      return false;
    case FINGERPRINT_FEATUREFAIL:
      return false;
    case FINGERPRINT_INVALIDIMAGE:
      return false;
    default:
      return false;
  }
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    return false;
  } else if (p == FINGERPRINT_NOTFOUND) {
    return false;
  } else {
    return false;
  }
   digitalWrite(buz,HIGH);
   delay(300);
   digitalWrite(buz,LOW);
  Serial.print("fingerindexId : "); Serial.println(finger.fingerID);
  return true;
}


/////////////////////////////////////////////////////////////////////////////////////



void getCardId(){
  while(1){
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  byte block;
  byte len;
  MFRC522::StatusCode status;
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    continue;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    continue;
  }
  digitalWrite(buz,HIGH);
  Serial.print("CardId :");
 for (int i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println(",");
  delay(300);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  digitalWrite(buz,LOW);
    break;
  }
   
}

/////////////////////////////////////////////////////////////////////////////////////


void send_sms(String no,String msg){
  HSerial1.println("AT"); //Handshaking with SIM900
  delay(1000);
  HSerial1.println("AT+CMGF=1"); // Configuring TEXT mode
  //Serial.println("AT+CMGF=1"); // Configuring TEXT mode
  delay(1000);
  HSerial1.println("AT+CMGS=\"+"+no+"\"");
  //Serial.println("AT+CMGS=\"+"+no+"\"");
  delay(1000);
  HSerial1.print(msg);
  //Serial.print(msg);
  delay(1000);
  HSerial1.write(26);
}

/////////////////////////////////////////////////////////////////////////////////////


void updateSerial()
{
  delay(500);
  while (HSerial1.available()) 
  {
    Serial.write(HSerial1.read());//Forward what Serial received to Software Serial Port
  }
}
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

/////////////////////////////////////////////////////////////////////////////////////


uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      digitalWrite(buz,HIGH);
      delay(300);
      digitalWrite(buz,LOW);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      digitalWrite(buz,HIGH);
      delay(300);
      digitalWrite(buz,LOW);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}
