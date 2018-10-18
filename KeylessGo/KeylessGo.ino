#define acc 5
#define ig1 4
#define ig2 3
#define st 2
#define clutchPin 7
#define seatbeltPin 6

#include <CurieBLE.h>

void setup() {
  pinMode(acc, OUTPUT);
  pinMode(ig1, OUTPUT);
  pinMode(ig2, OUTPUT);
  pinMode(st, OUTPUT);

  pinMode(clutchPin, INPUT);
  pinMode(seatbeltPin, INPUT);

  //Serial.begin(9600);

  // initialize the BLE hardware
  BLE.begin();

  // start scanning for peripheral
  BLE.scanForAddress("FD8D61F734CC", true);
}

int igState = 0;
int threshold = -60;
int lastKeyProx = -1;
void loop() {
  int tKP = keyProx();
  if(tKP!=-1){
    lastKeyProx = tKP;
  }
  //Serial.println(igState);
  //delay(250);
  switch(igState){
    case 0:
      if(lastKeyProx!=-1 && lastKeyProx>=threshold) keyAcc();
      break;
    case 1:
      if(lastKeyProx<threshold) keyOff();
      else if(digitalRead(clutchPin)==HIGH&&digitalRead(seatbeltPin)==LOW) keyStart();
      break;
    case 2:
      if(digitalRead(seatbeltPin)==HIGH){
        delay(50);
        if(digitalRead(seatbeltPin)==HIGH)keyAcc();
      }
      break;
    case 3:
      delay(1000);
      keyOn();
      break;
  }
}
int lastClutch = 0;
int clutchState(){ //TODO: Add debounce
  if(digitalRead(clutchPin)!= lastClutch){
    delay(10);
    if(digitalRead(clutchPin) != lastClutch) lastClutch = abs(lastClutch-1);
  }
  return lastClutch;
}

int lastSB = 0;
int seatbeltState(){ //TODO: Add debounce
  if(digitalRead(seatbeltPin)!= lastSB){
    delay(10);
    if(digitalRead(seatbeltPin) != lastSB) lastSB = abs(lastSB-1);
  }
  return lastSB;
}

int keyProx(){
  BLEDevice peripheral = BLE.available();
  if(peripheral) return peripheral.rssi();
  else return -1;  
}

void keyOff(){
  digitalWrite(acc, LOW);
  digitalWrite(ig1, LOW);
  digitalWrite(ig2, LOW);
  digitalWrite(st, LOW);
  igState = 0;
}

void keyAcc(){
  digitalWrite(acc, HIGH);
  digitalWrite(ig1, LOW);
  digitalWrite(ig2, LOW);
  digitalWrite(st, LOW);
  igState = 1;
}

void keyOn(){
  digitalWrite(acc, HIGH);
  digitalWrite(ig1, HIGH);
  digitalWrite(ig2, HIGH);
  digitalWrite(st, LOW);
  igState = 2;
}

void keyStart(){
  digitalWrite(acc, LOW);
  digitalWrite(ig1, HIGH);
  digitalWrite(ig2, LOW);
  digitalWrite(st, HIGH);
  igState = 3;
}
