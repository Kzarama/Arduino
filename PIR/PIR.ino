int mov = 0;

void setup() {
  pinMode(8, INPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  mov = digitalRead(8);
  if(mov == HIGH){
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    delay(1000);
  } else{
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
  }
  delay(100);
}
