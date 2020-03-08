#include <CapacitiveSensor.h>

CapacitiveSensor cs42 = CapacitiveSensor(4, 2);
CapacitiveSensor cs46 = CapacitiveSensor(4, 6);
CapacitiveSensor cs48 = CapacitiveSensor(4, 8);

void setup() {
  cs42.set_CS_AutocaL_Millis(0xFFFFFFFF);
  Serial.begin(9600);
  pinMode(8, OUTPUT);
}

void loop() {
 long sensor1 = cs42.capacitiveSensor(30);
 long sensor2 = cs46.capacitiveSensor(30);
 long sensor3 = cs48.capacitiveSensor(30);
 Serial.print(sensor1);
 Serial.print("_");
 Serial.print(sensor2);
 Serial.print("_");
 Serial.println(sensor3);
// if(sensor1 > 100){
//  Serial.println(sensor1);
//  digitalWrite(8, HIGH); 
// }else{
//  digitalWrite(8, LOW);
// }
 delay(100);
}
