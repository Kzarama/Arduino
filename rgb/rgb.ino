int rojo = 8;
int verde = 10;
int azul = 9;

void setup() {
  pinMode(rojo, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(azul, OUTPUT);
}

void loop() {
  analogWrite(azul, 0);
  analogWrite(rojo, 255);
  delay(500);
  analogWrite(rojo, 0);
  analogWrite(verde, 255);
  delay(500);
  analogWrite(verde, 0);
  analogWrite(azul, 255);
  delay(500);
  analogWrite(verde, 255);
  analogWrite(azul, 255);
  analogWrite(rojo, 255);
  delay(500);
  analogWrite(azul, 150);
  analogWrite(verde, 0);
  analogWrite(rojo, 0);
  delay(500);
}
