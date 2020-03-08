//#include "LedControl.h"
//
//const byte DIN = 12;
//const byte CS = 11;
//const byte CLK = 10;
//const byte QTD_DISP = 1;
//char state = 0;
//int bandera = 0;
//
//LedControl ledMatrix = LedControl(DIN, CLK, CS, QTD_DISP);
//
//void setup() {
//  Serial.begin(9600);
//  ledMatrix.shutdown(0, false);
//  ledMatrix.setIntensity(0, 5);
//  ledMatrix.clearDisplay(0);
//}
//
//void loop() {
//
//  if (Serial.available() > 0) {
//    state = Serial.read();
//    ledMatrix.clearDisplay(0);
//    ledMatrix.printChar(0, 0, state);
//    delay(500);
//  }
//  //  if (state == 'A' && bandera==0) {
//  //    digitalWrite(ledPin, HIGH);
//  //    bandera = 1;
//  //    state = 0;
//  //  }
//  //  if (state == 'A' && bandera==1) {
//  //     digitalWrite(ledPin, LOW);
//  //     bandera = 0;
//  //     state = 0;
//  //  }
//  //  char texto[] = state;
//
//
//}


#include "LedControl.h"

const byte DIN = 12;
const byte CS = 11;
const byte CLK = 10;
const byte QTD_DISP = 1;
String state = "";
int bandera = 0;

LedControl ledMatrix = LedControl(DIN, CLK, CS, QTD_DISP);

void setup() {  
  Serial.begin(9600);
  ledMatrix.shutdown(0, false);
  ledMatrix.setIntensity(0, 5);
  ledMatrix.clearDisplay(0);  
}

void loop(){
  boolean pass = false;
  int numero = 0;
  if(Serial.available() > 0){
    state = Serial.readString();
    if(state.lastIndexOf('.') != -1){
      pass = true;
    }
  }
  char ms[state.length()+2];
  for(int i = 0; i < state.length(); i++){
    ms[i] = state.charAt(i);
  }
  ms[state.length()+1] = ' ';
  ms[state.length()+2] = ' ';
  if(pass){
    ledMatrix.clearDisplay(0);
    ledMatrix.printStringScroll(0, 8, ms, 100, '<');
    delay(500);
    pass = false;
  }
}
