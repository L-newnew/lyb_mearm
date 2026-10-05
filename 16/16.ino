#include <Servo.h>
#define tt 50                        //运动周期
#define d 1                           //步长
int aa[4] = { 89, 98, 93,90 };        //目标角
unsigned long last[4] = { 0, 0, 0,0 };  //上次执行时间点
Servo x[4];                           //三个电机
int s[3];                             //路程
int t[4] = { tt, tt, tt,2};        //实际运动周期
int ss;                               //最长路程
int ua[3];                            //由摇杆电压值换算得到的角度值
int pinA[3] = { A0, A1, A2 };         //摇杆信号接收管脚
int zhua = 0;                           //夹爪状态（0或1）
int t_zhua=500;                         //夹爪按键响应周期
unsigned long last_zhua=0;
void setup() {
  Serial.begin(9600);
  x[0].attach(9);
  x[1].attach(10);
  x[2].attach(11);
  x[3].attach(6);
  pinMode(8,INPUT_PULLUP);
}


void loop() {
  if(digitalRead(8)==0)//读取按键信号，当按键被按下
  {
    if(millis()-last_zhua>=t_zhua)    
    {
      zhua=1-zhua;
      if(zhua==1)aa[3]=140;
      else aa[3]=30;
      last_zhua=millis();
    }
  }



  for (int i = 0; i <= 2; i++)  //读取摇杆信号
  {
    ua[i] = g(analogRead(pinA[i]));
    //ua[i] = analogRead(pinA[i]);
  }

  if (abs(ua[0]-aa[0])>2||abs(ua[1]-aa[1])>2||abs(ua[2]-aa[2])>2)  //当摇杆信号发生较大改变         //不要写ua[]!aa[]||...||...
  {
    for (int i = 0; i <= 2; i++)  //修改目标角
    {
      aa[i] = ua[i];
    }
    //反馈
    Serial.print(aa[0]);
    Serial.print(" ");
    Serial.print(aa[1]);
    Serial.print(" ");
    Serial.println(aa[2]);
    //注意：触发目标角修改后才进入实际周期计算

    ss = 0;  //算路程
    for (int i = 0; i <= 2; i++) {
      s[i] = abs(x[i].read() - aa[i]);
      if (s[i] >= ss) ss = s[i];
    }

    for (int i = 0; i <= 2; i++)  //算实际周期,改实际周期
    {
      if (s[i] != 0) t[i] = ss * tt / s[i];
      last[i] = millis();
    }
  }

  for (int i = 0; i <= 3; i++)  //按实际周期、目标角驱动各个电机
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

int g(int x)  //换算
{
  return (float)x / 1023 * 180;
}