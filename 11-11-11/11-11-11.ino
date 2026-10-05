#include<Servo.h>
Servo a;
unsigned long lasttime=0;//ms,上次执行时间点
int t=50;//ms,周期
int d=1;//度数，步长
int angle=90;//当前角度
int k=0;



//串口输入a来终止，输入b继续
void setup()
{
  Serial.begin(9600);
  a.attach(9);
  a.write(angle);
  a.write(90);
}

void loop() 
{
  
  if(Serial.available()!=0)
  {
    char c=Serial.read();
    if(c=='a')k=0;
    else if(c=='b')k=1;
  }


  if(millis()-lasttime>=t&&k==1)
  {
    if(angle<120&&angle>30)
      angle+=d;
    else 
      {d=d*-1;angle+=d;}
    
    a.write(angle);

    lasttime=millis();
  } 
   

}
