boolean a = true;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if(a){
    punto();
    punto();
    punto();
    punto();
    linea();
    linea();
    linea();
    punto();
    linea();
    punto();
    punto();
    punto();
    linea();
    linea();
    linea();
    punto();
    punto();
    linea();
    linea();
    punto();
    linea();
    punto();
    punto();
    linea();
    linea();
    linea();
    punto();
    linea();
    punto();
    linea();
    punto();
    a = true;
  }
}

void linea(){
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

void punto(){
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
