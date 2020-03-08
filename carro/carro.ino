int state = 0;

void setup() {
//  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  pinMode(11, OUTPUT);
  digitalWrite(11, LOW);
  Serial.begin(9600);
}

void loop() {
  if(Serial.available() > 0){
    state = Serial.read();
  }
  if(state == 'A') {
    avanzar();
//    parar();
    state = 0;
  }
  state = Serial.read();
  if(state == 'B') {
    retroceder();
//    parar();
    state = 0;
  }
  state = Serial.read();
  if(state == 'C') {
    izq();
//    parar();
    state = 0;
  }
  state = Serial.read();
  if(state == 'D') {
    der();
//    parar();
    state = 0;
  }
}

//////////////////////////////////////////////////////

void avanzar(){
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  digitalWrite(11, LOW);
  digitalWrite(10, HIGH);
  delay(1000);
}

void retroceder(){
  digitalWrite(9, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(10, LOW);
  digitalWrite(11, HIGH);
  delay(1000);
}

void izq(){
  
}

void der(){
  
}

void parar(){
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}
