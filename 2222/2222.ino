#include <Servo.h>
Servo a,b,c;
void setup() {
  // put your setup code here, to run once:
a.attach(9);
b.attach(10);
c.attach(11);
}

void loop() {
  // put your main code here, to run repeatedly:
a.write(0);
b.write(0);
c.write(0);
delay(1000);


a.write(180);
b.write(180);
c.write(180);
delay(1000);
}
