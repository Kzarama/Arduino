//VT En Liena
//by Jorge

#include <DFPlayer_Mini_Mp3.h>
#include <CapacitiveSensor.h>
#include <SoftwareSerial.h>
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2);
CapacitiveSensor   cs_4_6 = CapacitiveSensor(4,6);
CapacitiveSensor   cs_4_8 = CapacitiveSensor(4,8);
SoftwareSerial     mySerial(10, 11); // RX, TX

int playpause = 0;
int Trig = A2;
int Echo = A0;

void setup()                    
{
   cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);
   Serial.begin(9600);
   mySerial.begin (9600);
   pinMode(Trig,OUTPUT);
   pinMode(Echo,INPUT);
   pinMode(A4,INPUT);
   mp3_set_serial (mySerial);
   delay(1);
   mp3_set_volume (15);
}

void loop()                    
{
  long sige =  cs_4_2.capacitiveSensor(30);
  long play =  cs_4_6.capacitiveSensor(30);
  long ante =  cs_4_8.capacitiveSensor(30);
  int volumen;
  int siguiente;

  if(playpause == 1)
  {
    volumen = ultra();
    siguiente = digitalRead(A4);
    if(siguiente == HIGH)
      mp3_next ();
      
    if(volumen < 35 && volumen > 5)
    {
      volumen = volumen - 5;
      mp3_set_volume (volumen);
      Serial.println(volumen);
    }
    delay(50);
  }

  if(sige > 50)
  {
    Serial.println("siguiente");
    mp3_next ();
    playpause = 1;
    while(sige > 30)
    {
      sige =  cs_4_2.capacitiveSensor(30);
      delay(100);
    }
  }
  
  else if(play > 50)
  {
    if(playpause == 0)
    {
      Serial.println("Play");
      mp3_play ();
      playpause = 1;
    }
    else
    {
      Serial.println("Pausar");
      mp3_pause ();
      playpause = 0;
    }
    while(play > 30)
    {
      play =  cs_4_6.capacitiveSensor(30);
      delay(100);
    }
  }
  
  else if(ante > 50)
  {
    Serial.println("anterior");
    mp3_prev ();
    playpause = 1;
    while(ante > 30)
    {
      ante =  cs_4_8.capacitiveSensor(30);
      delay(100);
    }
  }

  delay(10);
}

int ultra()
{
  long duracion;
  long distancia;
  
  digitalWrite(Trig,LOW);
  delayMicroseconds(2);
  digitalWrite(Trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig,LOW);
  duracion = pulseIn(Echo,HIGH);
  distancia = (duracion/2)/29;
  return distancia;
}

