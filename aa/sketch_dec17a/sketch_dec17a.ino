int PinIn1 = 7;
int PinIn2 = 6;
int PinIn3 = 5;
int PinIn4 = 4;

void setup() {
 Serial.begin(9600);
 pinMode(PinIn1, OUTPUT);
 pinMode(PinIn2, OUTPUT);
}

void loop() {
  if (Serial.available()){
    char dato = ' ';
    dato = Serial.read();
    if (dato == 'A'){
      avanzar();
    }
    if (dato == 'I'){
      Serial.println("i");
    }
    if (dato == 'D'){
      Serial.println("d");
    }
    if (dato == 'R'){
      reversa();
    }
  }
}

void avanzar(){
  digitalWrite(PinIn1, LOW);
  digitalWrite(PinIn2, HIGH);
}

void izquierda(){
  
}

void derecha(){
  
}

void reversa(){
  digitalWrite(PinIn2, LOW);
  digitalWrite(PinIn1, HIGH);
}
