#include <Servo.h>
Servo x[3];

int f(int x)
{
  return (float)x/1023*180;
}
int aa[3]={89,98,93};
int ua[3];//由摇杆电压值换算得到的角度值
int pinA[3]={A0,A1,A2};//摇杆信号接收管脚

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
x[0].attach(9);
x[1].attach(10);
x[2].attach(11);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0;i<=2;i++)
  {
    ua[i]=f(analogRead(pinA[i]));
    x[i].write(ua[i]);
  }


Serial.print(ua[0]);
Serial.print(" ");
Serial.print(ua[1]);
Serial.print(" ");
Serial.println(ua[2]);

}
