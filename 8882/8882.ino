#include <Servo.h>
char a;
float b,v,bb=0;;//b表示目标角度；v表示每1ms转动角度数；bb表示当前角度；
Servo x,y;




void setup() 
{
Serial.begin(9600);
x.attach(9);
x.write(0);
y.attach(10);
y.write(0);
}





void loop() 
{

if(Serial.available()!=0)//输入格式：x角度v速度
{
  a=Serial.read();
  if(a=='x')
  b=Serial.parseFloat();
  a=Serial.read();
  if(a=='v')
  v=Serial.parseFloat();
 

  Serial.read();
  Serial.print(b);
  Serial.print(" ");
  Serial.println(v);
  
  f(y,b,v);


}
delay(300);
}

void f(Servo x,float b,float v){
  float bb = x.read();
  while(1)
  {
    if(bb<b-v){x.write(bb+v);bb=bb+v;}
    else if(bb>b+v){x.write(bb-v);bb=bb-v;}
    else {x.write(b);bb=b;}
    if (bb==b) break;
    delay (1);
  }
}