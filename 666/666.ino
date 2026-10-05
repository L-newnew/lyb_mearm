#include <Servo.h>
char a;
int b,c,d;
Servo x,y,z;
void setup() 
{
Serial.begin(9600);
x.attach(9);
y.attach(10);
z.attach(11);
}





void loop() 
{

if(Serial.available()!=0)
{
  a=Serial.read();
  if(a=='x')
  b=Serial.parseInt();
  
  a=Serial.read();
    if(a==',')
  {  a=Serial.read();
    if(a=='y')
  c=Serial.parseInt();}

a=Serial.read();
    if(a==',')
  {  a=Serial.read();
    if(a=='z')
  d=Serial.parseInt();}


 Serial.read();

  Serial.print(b);
  Serial.print(" ");
  Serial.print(c);
  Serial.print(" ");
  Serial.println(d);

  x.write(b);
  y.write(c);
  z.write(d);
}
delay(300);
}
