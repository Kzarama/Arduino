int PinIn1 = 7;
int PinIn2 = 6;

void setup() {
 Serial.begin(9600);
 pinMode(PinIn1, OUTPUT);
 pinMode(PinIn2, OUTPUT);
}

void loop() {
  char dato = Serial.read();
  if (dato == 'A'){
    Serial.println(dato + "a");
  }
  if (dato == 'I'){
    Serial.println(dato + "i");
  }
  if (dato == 'D'){
    Serial.println(dato + "d");
  }
  if (dato == 'R'){
    Serial.println(dato + "r");
  }
}
