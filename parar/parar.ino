void setup() {
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
}

void loop() {
  colors(1,1,1);
  delay(1000);
  colors(1,1,0);
  delay(1000);
  colors(1,0,1);
  delay(1000);
  colors(0,1,1);
  delay(1000);
  colors(1,0,0);
  delay(1000);
  colors(0,1,0);
  delay(1000);
  colors(0,0,1);
  delay(1000);
}

void colors(int a, int b, int c){
  if (a == 1){
    digitalWrite(8, HIGH);
  } else {
    digitalWrite(8, LOW);
  }
  if (b == 1){
    digitalWrite(9, HIGH);
  } else {
    digitalWrite(9, LOW);
  }
  if (c == 1){
    digitalWrite(10, HIGH);
  } else {
    digitalWrite(10, LOW);
  }
}
