#include <Servo.h>
#define tt 50                            //运动周期
#define d 1                              //步长
int aa[4] = { 89, 98, 93, 90 };          //目标角
unsigned long last[4] = { 0, 0, 0, 0 };  //上次执行时间点
Servo x[4];                              //三个电机
int t[4] = { tt, tt, tt, 2 };            //实际运动周期
int ua[3];                               //由摇杆电压值换算得到的角度值
int pinA[3] = { A0, A1, A2 };            //摇杆信号接收管脚
//int zhua = 0;                       //夹爪状态（0或1）

int t_an = 500;             //按键响应周期
unsigned long last_an = 0;  //按键上次响应时间

int pa[101][3] = { { 89, 98, 93 } };  //第一个表示初始位置的角度，后面的表示记录下来的角度
int k = 0;                            //按键次数
int t_move = 100;                     //姿态记录（动作命令发出）周期
unsigned long last_move = 0;          //上次记录姿态（发出动作命令）时间
int move = 1;                         //计数
int kk = 0;                           //状态（kk=0：等待；kk=1：录制；kk=2：播放）

//开始时按下按键进入录制阶段（本质：每100ms进行一次记录），往后每次按下按键进行播放（本质：每100ms发出一次命令）。

void setup() {
  Serial.begin(9600);
  x[0].attach(9);
  x[1].attach(10);
  x[2].attach(11);
  x[3].attach(6);
  pinMode(8, INPUT_PULLUP);
}


void loop() {
  if (digitalRead(8) == 0)  //读取按键信号，当按键被按下
  {
    if (millis() - last_an >= t_an) {
      if (k == 0)  //进入录制模式
      {
        kk = 1;
        last_move = millis();
      } else  //进入播放模式
      {
        kk = 2;
        last_move = millis();
      }
      last_an = millis();
      k++;
    }
  }



  if (millis() - last_move >= t_move) {
    if (move == 101) {
      for (int i = 0; i <= 2; i++) {
        aa[i] = pa[0][i];
        back(aa, 3);
        set_t(t, 3, aa, 3, last, 3, x, 3);
      }
      kk = 0;
      move = 1;
    }


    if (kk == 1)  //当进入录制模式
    {
      for (int i = 0; i <= 2; i++) {
        pa[move][i] = aa[i];
      }
      move++;
      last_move = millis();
    }

    else if (kk == 2)  //当进入播放模式
    {

      if (millis() - last_move >= t_move) {
        for (int i = 0; i <= 2; i++) {
          aa[i] = pa[move][i];
          back(aa, 3);
          set_t(t, 3, aa, 3, last, 3, x, 3);
        }
        
        move++;
        last_move = millis();
      }
    }
  }


  for (int i = 0; i <= 2; i++)  //读取摇杆信号
  {
    ua[i] = g(analogRead(pinA[i]));
    //ua[i] = analogRead(pinA[i]);
  }

  if ((abs(ua[0] - aa[0]) > 2 || abs(ua[1] - aa[1]) > 2 || abs(ua[2] - aa[2]) > 2) && k == 1)  //当摇杆信号发生较大改变（录制阶段） //不要写ua[]!aa[]||...||...
  {
    for (int i = 0; i <= 2; i++)  //修改目标角
    {
      aa[i] = ua[i];
    }
    back(aa, 3);
    set_t(t, 3, aa, 3, last, 3, x, 3);
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

void back(int aa[], int aan)  //反馈三关节的目标角
{
  Serial.print(aa[0]);
  Serial.print(" ");
  Serial.print(aa[1]);
  Serial.print(" ");
  Serial.println(aa[2]);
}
//back(aa,3);

void set_t(int t[], int tn, int aa[], int aan, unsigned long last[], int lastn, Servo x[], int xn)  //算路程、算实际周期、改实际周期
                                                                                                    //函数调用前提：触发三关节目标角修改
{
  int ss = 0;                   //最长路程
  int s[3] = { 0, 0, 0 };       //路程
  for (int i = 0; i <= 2; i++)  //算路程
  {
    s[i] = abs(x[i].read() - aa[i]);
    if (s[i] >= ss) ss = s[i];
  }

  for (int i = 0; i <= 2; i++)  //算实际周期,改实际周期
  {
    if (s[i] != 0) t[i] = ss * tt / s[i];
    last[i] = millis();
  }
}
//set_t(t,3,aa,3,last,3,x,3);



//### 播放时没有上限检查

//录制时你可能只录了 30 个点位就按了第二次键，但播放时 `move` 从 1 一直往上走，走到 31 以后读的是 `pa[31]~pa[100]`—— 这些都是 **0**（没录过），臂会突然冲到 0,0,0，可能撞坏。

//**解决**：加一个变量记录实际录了多少个点：
