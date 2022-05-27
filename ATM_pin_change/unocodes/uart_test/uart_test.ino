#include <HardwareSerial.h>
#include <Adafruit_Fingerprint.h>

#define BSerial Serial2
#define buz 4

HardwareSerial HSerial1(1);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&BSerial);


void setup() {
    HSerial1.begin(9600, SERIAL_8N1, 13, 12);
    Serial.begin(9600);
    finger.begin(57600);
    delay(5);
      if (finger.verifyPassword()) {
    ;
  } else {
    while (1) { delay(1); }
  }
  pinMode(buz,OUTPUT);
  digitalWrite(buz,LOW);
  
}

void loop() {
  String data = "";
  if(Serial.available()){
    data = Serial.readString();
    if(data.indexOf("getcardid") != -1)Serial.println("CardId:12345678;");
    if(data.indexOf("getfingerindex") != -1)get_index();
    if(data.indexOf("otp:") != -1){
      int otp = data.substring(data.indexOf("otp:")+4,data.indexOf(',')).toInt();
      Serial.print("OTP : ");
      Serial.println(otp);
    }
    
    //HSerial1.println(data);
  }
    if(HSerial1.available()){
    data = HSerial1.readString();
    Serial.println(data);
  } 
    //getFingerprintID();
    
  delay(50); 
}

void get_index(){
while(1){
  if(getFingerprintID())break;
}
delay(50);
}

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
void send_sms(String no,String msg){
  Serial1.println("AT"); //Handshaking with SIM900
  delay(1000);
  Serial1.println("AT+CMGF=1"); // Configuring TEXT mode
  delay(1000);
  Serial1.println("AT+CMGS=\"+"+no+"\"");
  delay(1000);
  Serial1.print(msg); 
  delay(1000);
  Serial1.write(26);
}





uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
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
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
