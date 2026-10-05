#include <Servo.h>
#define tt 40                        //运动周期
#define d 1                           //步长
int aa[3] = { 89, 98, 93 };           //目标角
unsigned long last[3] = { 0, 0, 0 };  //上次执行时间点
Servo x[3];                           //三个电机
int s[3];                             //路程
int t[3] = { tt, tt, tt };            //实际运动周期
int ss;


void setup() {
  Serial.begin(9600);
  x[0].attach(9);
  x[1].attach(10);
  x[2].attach(11);
}


void loop() {

  if (Serial.available() != 0)  //如果输入目标角
  {
    //修改目标角
    if (Serial.read() == 'x') { aa[0] = Serial.parseInt(); }
    if (Serial.read() == 'y') { aa[1] = Serial.parseInt(); }
    if (Serial.read() == 'z') { aa[2] = Serial.parseInt(); }
    Serial.read();
    //反馈
    Serial.print("x");
    Serial.print(aa[0]);
    Serial.print("y");
    Serial.print(aa[1]);
    Serial.print("z");
    Serial.println(aa[2]);

    //注意：触发目标角修改后才进入实际周期计算

    ss = 0;  //算路程
    for (int i = 0; i <= 2; i++) {
      s[i] = abs(x[i].read() - aa[i]);
      if (s[i] >= ss) ss = s[i];
    }

    for (int i = 0; i <= 2; i++)  //算周期,改实际周期
    {
      if (s[i] != 0) t[i] = ss * tt / s[i];
      last[i] = millis();
    }
  }

  for (int i = 0; i <= 2; i++)  //按实际周期、目标角驱动各个电机
  {
    f(x[i], aa[i], t[i], &last[i]);
  }
}

void f(Servo x, int aa, int t, unsigned long *last)  //令单个电机以设定周期运动到设定终点
{
  int k, a = x.read();
  if (aa != a && millis() - *last >= t) {
    if (aa > a) k = 1;
    else if (aa < a) k = -1;
    if (abs(aa - a) < d) a = aa;
    else a = a + d * k;
    x.write(a);
    *last = millis();
  }
}