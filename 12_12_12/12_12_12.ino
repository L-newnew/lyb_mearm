#include <Servo.h>
#define d 1
#define t 50//ms
int last_time=0;
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}


void f(Servo x,int aa,int *last_time)
{
  int k,a=x.read();
  if(aa!=a&&millis()-*last_time>=t)
  {
    if(aa>a)k=1;
    else if(aa<a)k=-1;
    if(abs(aa-a)<d)a=aa;
    else a=a+d*k;
    x.write(a);
    *last_time=millis();
  }
}