#include "LedControl.h"     //simpre incluimos la libreria de control 

const byte DIN      = 12;   //Lo conectamos en din
const byte CS       = 11;   //Lo conectamos a Load (cs)
const byte CLK      = 10;   //Lo conectarmos a CLK 
const byte QTD_DISP =  1;   //El nuemro de matriz con controlador M72XX

LedControl ledMatrix = LedControl(DIN, CLK, CS, QTD_DISP);

void setup() {
  
 // El MAX72XX está en modo de ahorro de energía en el arranque, tenemos que hacer que despierte
  ledMatrix.shutdown(0, false);  //modo 'shutdown' no display '0' y FALSE
  
// Establecer el brillo a un valor medio
  ledMatrix.setIntensity(0, 5);  //intensidad del display '0' y 5 (0~16)
  
  // y borramos la pantalla
  ledMatrix.clearDisplay(0);     //borrar pantalla '0'
  
}

void loop(){
  
  //byte i=0, col=0, row=0;
  //char texto[] = "mo :D   ";
  
  //ledMatrix.clearDisplay(0);     //borrar pantalla '0'
  //ledMatrix.printChar(0, 0, 'K');
  //delay(500);
  //ledMatrix.printChar(0, 0, 'Z');
  //delay(500);
  
  //ledMatrix.clearDisplay(0);     //borrar pantalla '0'
  //ledMatrix.printString(0, 0, "KZ");
  //delay(500);
  
  
  ledMatrix.clearDisplay(0);
  //ledMatrix.printStringScroll(0, 8, texto, 100, '<');
  //delay(500);
  //ledMatrix.printStringScroll(0, 0, texto, 50, '>');
  //delay(500);
  
}
