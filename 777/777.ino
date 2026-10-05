#include<Servo.h>
Servo w;
char a;



void setup() {
w.attach(9);
Serial.begin(9600);
}

void loop() {

if(Serial.available()!=0)
{
 a=Serial.read();
if(a=='o')
w.write (180);
else if(a=='s')
w.write(0);

 Serial.read();
}
delay(500);
}
