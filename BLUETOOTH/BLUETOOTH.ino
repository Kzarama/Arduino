int ledPin = 13;
int state = 0;
int bandera = 0;

void setup() {
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    Serial.begin(9600);
}

void loop() {
  if(Serial.available() > 0){
    state = Serial.read();
  }
  if (state == 'A' && bandera==0) {
    digitalWrite(ledPin, HIGH);
    bandera = 1;
    state = 0;
  }
  if (state == 'A' && bandera==1) {
     digitalWrite(ledPin, LOW);
     bandera = 0;
     state = 0;
  }

}
