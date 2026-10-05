#include <Servo.h>
#define tt 50                            //运动周期
#define d 1                              //步长
int aa[4] = { 89, 98, 93, 90 };          //目标角
unsigned long last[4] = { 0, 0, 0, 0 };  //上次执行时间点
Servo x[4];                              //三个电机
int t[4] = { tt, tt, tt, 2 };            //实际运动周期
int ua[3];                          //由摇杆电压值换算得到的角度值
int pinA[3] = { A0, A1, A2 };       //摇杆信号接收管脚
int t_an = 500;                                //按键响应周期
unsigned long last_an[4] = {0,0,0,0};          //按键上次响应时间
int pa[7][3] = { { 89, 98, 93 } };  //第一个表示初始位置的角度，后六个表示记录下来的角度（模式1）
int ppa[101][3] = { { 89, 98, 93 } }//第一个表示初始位置的角度，后六个表示记录下来的角度（模式2）
int k1 = 0;                          //模式1记录点数
int k2 = 0;                          //模式2录制点数
int k3 = 0;                          //模式3播放点数
int t_record =100;                  //姿态录制（动作命令发出）周期（模式2、3）
unsigned long last_record = 0;        //姿态上次录制（动作命令上次发出）时间点（模式2、3）
int t_move = 5000;                  //动作命令发出周期（模式1）
unsigned long last_move = 0;        //动作命令上次发出时间点（模式1）
int step_move = 1;                             //发令编号（模式1）
int playback = 0;                              //是否进入复现过程（模式1）
int mode=4;          //模式  1自动夹取；2录制；3播放；4复位

void setup() 
{
  Serial.begin(9600);
  x[0].attach(9);
  x[1].attach(10);
  x[2].attach(11);
  x[3].attach(6);
  pinMode(1,INPUT_PULLUP);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);


}

void loop()
{
  if (digitalRead(4) == 0)  //读取按键4信号，当按键4被按下，进入模式4
  {
    if (millis() - last_an[3] >= t_an)
    {
      mode=4;

      last_an[3] = millis();
    }
  }
  if(mode==4)//当进入模式4，改变目标角
  {
    for(int i=0;i<=2;i++)
    {
      aa[i]=pa[0][i];
      back(aa,3);
      set_t(t,3,aa,3,last,3,x,3);
    }
  }

  if (digitalRead(3) == 0)  //读取按键3信号，当按键3被按下，进入模式3
  {
    if (millis() - last_an[2] >= t_an)
    {
      mode=3;
      k3=0;
      last_record=millis();

      last_an[2] = millis();
    }
  }
  
  if (digitalRead(2) == 0)  //读取按键2信号，当按键2被按下，进入模式2
  {
    if (millis() - last_an[1] >= t_an)
    {
      mode=2;
      k2=0;
      last_record=millis();

      last_an[1] = millis();
    }
  }
  if(mode==2)//按周期用pa[][3]来存目标角
  {
    if(millis()-last_record>=t_record)
    {
      if(k2<100)
      {
        for (int i = 0; i <= 2; i++) 
        {
          ppa[k2+1][i] = aa[i];
        }
        last_record=millis();
        k2++;
      }
      else 
      mode=4;
    }
  }
  else if(mode==3)//按周期改变目标角
  {
    if(millis()-last_record>=t_record)
    {
      if(k3<=k2)
      {
        for (int i = 0; i <= 2; i++) 
        {
          aa[i]=ppa[k2][i] ;
          back(aa,3);
          set_t(t,3,aa,3,last,3,x,3);
        }
        last_record=millis();
        k3++;
      }
      else
      mode=4;
      
    }
  }
  
  if (digitalRead(1) == 0)  //读取按键1信号，当按键1被按下
  {
    if (millis() - last_an[0] >= t_an)
    {
      mode=1;
      if (k1 < 6)  //记录电机角度（六次）
      {
        for (int i = 0; i <= 2; i++) 
        {
          pa[k + 1][i] = aa[i];
        }
        k1++;
      } 
      else  //激活动作再现
      {
        playback = 1;
        last_move = millis();  
      }
      
      last_an[0] = millis();
    }
  }
  if(mode==1)
  {
    if (playback == 1)  //当动作复现被激活,按周期改变目标角
    {
      else if (millis() - last_move >= t_move)
      {
        move_(aa,4,&step_move,&playback);
        back(aa,3);
        set_t(t,3,aa,3,last,3,x,3);
        last_move=millis();
      }
    }
  }
  else playback=0;
  
  for (int i = 0; i <= 2; i++)  //读取摇杆信号
  {
    ua[i] = g(analogRead(pinA[i]));
    //ua[i] = analogRead(pinA[i]);
  }

  if ((abs(ua[0] - aa[0]) > 2 || abs(ua[1] - aa[1]) > 2 || abs(ua[2] - aa[2]) > 2) && (k < 6&&mode==1)||mode==2)
  //当摇杆信号发生较大改变(模式1记录期及模式2) 
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

void set_t(int t[], int tn, int aa[], int aan, unsigned long last[], int lastn, Servo x[], int xn) //算路程、算实际周期、改实际周期   
 //函数调用前提：触发三关节目标角修改
{
  int ss = 0;           //最长路程
  int s[3]={0,0,0};      //路程
  for (int i = 0; i <= 2; i++)//算路程 
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

void move_(int aa[], int aan,int *step_move,int *playback)//依靠指令发出计数（step_move），修改目标角aa[]并管理playback、step_move值（模式1复现期）
{
  if(*step_move%5==1)//移动至p1、p3、p5
  {
    for(int i=0;i<=2;i++)
    {
      aa[i]=pa[*step_move*0.4+0.6][i];
    }
  }
  else if(*step_move%5==2)//合爪
  {
    aa[3] = 60;
  }
  else if(*step_move%5==3)//移动至p2、p4、p6
  {
    for(int i=0;i<=2;i++)
    {
      aa[i]=pa[*step_move*0.4+0.8][i];
    }
  }
  else if(*step_move%5==4)//开爪
  {
    aa[3] = 120;
  }
  else if(*step_move%5==0)//复位
  {
    for(int i=0;i<=2;i++)
    {
      aa[i]=pa[0][i];
      *playback=0;
    }
  }
  if(*step_move==15)
  {
    *step_move=0;
  }
  *step_move++;
}
// move_(aa,4,&step_move,&playback);