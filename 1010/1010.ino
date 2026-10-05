#include<Servo.h>
Servo a,b,c;

int aa,bb,cc;
void setup() {
  // put your setup code here, to run once:
 a.attach(9);
 b.attach(10);
 c.attach(11);
 Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
if(Serial.available()!=0)
{
   
  if(Serial.read()=='x')
  aa=Serial.parseInt();
  
  if(Serial.read()=='y')
  bb=Serial.parseInt();

  if(Serial.read()=='z')
  cc=Serial.parseInt();
  Serial.read();
  f(a,b,c,aa,bb,cc);
}

}
void f(Servo a,Servo b,Servo c,int aa,int bb,int cc)
{
  a.write(aa);
  delay(500);
  b.write(bb);
  delay(500);
  c.write(cc);
  delay(500);
}