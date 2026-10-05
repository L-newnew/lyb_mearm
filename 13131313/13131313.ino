#include <Servo.h>
#define d 1//步长
#define t 80//周期
Servo x,y,z;
unsigned long last=0;
int aa[3]={89,98,93};//不要写000！


void setup()
{
Serial.begin(9600);
x.attach(9);y.attach(10);z.attach(11);

}

void loop() 
{
  if(Serial.available()!=0)
{
  if(Serial.read()=='x')
  {aa[0]=Serial.parseInt();}
  if(Serial.read()=='y')
  {aa[1]=Serial.parseInt();}
  if(Serial.read()=='z')
  {aa[2]=Serial.parseInt();}
  Serial.read();
  Serial.print("x");
  Serial.print(aa[0]);
  Serial.print("y");
  Serial.print(aa[1]);
  Serial.print("z");
  Serial.println(aa[2]);
}
g(x,y,z,aa,3,&last);

}


void f(Servo x,int aa)
{
  int k,a=x.read();
  if(aa!=a)
  {
    if(aa>a)k=1;
    else if(aa<a)k=-1;
    if(abs(aa-a)<d)a=aa;
    else a=a+d*k;
    x.write(a);
  }
}


void g(Servo x,Servo y,Servo z,int aa[],int aan,unsigned long *last_time)
{
  if(millis()-*last_time>=t)
  {
    f(x,aa[0]);
    f(y,aa[1]);
    f(z,aa[2]);

    *last_time=millis();
  }      
}