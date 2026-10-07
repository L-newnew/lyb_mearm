#include <Servo.h>
int tt =50;                           //运动周期
#define d 1                              //步长
int aa[4] = { 89, 98, 93, 90 };          //目标角
unsigned long last[4] = { 0, 0, 0, 0 };  //上次执行时间点
Servo x[4];                              //三个电机
int t[4] = { tt, tt, tt, 2 };            //实际运动周期
int ua[3];                          //由摇杆电压值换算得到的角度值
int pinA[3] = { A0, A1, A2 };       //摇杆信号接收管脚
int t_an = 500;                                //按键响应周期
unsigned long last_an[7] = {0,0,0,0,0,0,0};          //按键上次响应时间
int pa[7][3] = { { 89, 98, 93 } };  //第一个表示初始位置的姿态，后六个表示记录下来的姿态（模式1）
int ppa[101][3] = { { 89, 98, 93 } };//第一个表示初始位置的姿态，后面表示记录下来的姿态（模式2）
int k1 = 0;                          //模式1记录点数
int k2 = 0;                          //模式2录制点数
int k3 = 0;                          //模式3播放点数
int k4 = 0;                            //模式6记录点数
int t_record =100;                  //姿态录制（动作命令发出）周期（模式2、3）
unsigned long last_record = 0;        //姿态上次录制（动作命令上次发出）时间点（模式2、3）
int t_move = 4000;                  //动作命令发出周期（模式1）
unsigned long last_move = 0;        //动作命令上次发出时间点（模式1）
int step_move = 1;                             //发令编号（模式1）
int playback = 0;                              //是否进入复现过程（模式1）
int mode=5;          //模式  1自动夹取；2录制；3播放；4复位     5自由模式（任务一演示） 6两点记录 7两点成线
int pppa[3]={0,0,0};//暂存串口输入（模式5）

int ppppa[2][3]={0,0,0};//记录两点姿态(模式6)
int t_push = 100;                  //动作命令发出周期（模式7）
unsigned long last_push = 0;        //动作命令上次发出时间点（模式7）

#define l1 8
#define l2 8
#define r 1.45
double pp[3]={0,0,0};//目标位置坐标（模式7）

//double a[3]绝对角(中间量)（模式7）
double pppp[2][3]={0,0,0};//记录两点姿态（ppppa）对应的坐标（模式7）
//double ppp[3]记录两点的位移量（中间量）（模式7）
//double long_ppp最长位移分量（中间量）（模式7）
double p[3]= {0,0,0};//画线阶段位移微量（模式7）

void setup() 
{
  Serial.begin(9600);
  x[0].attach(9);
  x[1].attach(10);
  x[2].attach(11);
  x[3].attach(6);
  pinMode(5,INPUT_PULLUP);//按键1
  pinMode(2,INPUT_PULLUP);//按键2
  pinMode(3,INPUT_PULLUP);//按键3
  pinMode(4,INPUT_PULLUP);//按键4
  pinMode(7,INPUT_PULLUP);//按键5
  pinMode(8,INPUT_PULLUP);//按键6
  pinMode(12,INPUT_PULLUP);//按键7
}

void loop()
{

  if (digitalRead(12) == 0)  //读取按键7信号，当按键7被按下，若已有两个记录点进入模式7
  {
    if (millis() - last_an[6] >= t_an)
    {
      Serial.print(777777);//反馈：按键7已响应
      if(k4==2)
      {
        mode=7;
        Serial.println("yes");//反馈:已进入模式七
        last_push=millis()+4000;//给足时间就位
        for(int i=0;i<=2;i++)//就位
        {
          aa[i]=ppppa[0][i];
        }
        hhh(p,3);//确定位移微量
        for(int i=0;i<=2;i++)
        {
          pp[i]=pppp[0][i];
        }
      }
      else
      {
        Serial.println("no");//反馈：无法进入模式七
      }


      last_an[6] = millis();
    }
  }
  if(digitalRead(8) == 0)//读取按键6信号，当按键6被按下，进入模式6
  {
    if (millis() - last_an[5] >= t_an)
    {
      Serial.println(66666);//反馈：按键6已响应
      if(mode==6&&k4<2)//未存满两个点
      {
        for(int i=0;i<=2;i++)
        {
          ppppa[k4][i]=x[i].read();
        }
        k4++;
        Serial.println(k4);//反馈：存了多少个点
      }
      else if(mode==6&&k4==2)//已存满两个点，重新开始存
      {
        k4=0;
        for(int i=0;i<=2;i++)
        {
          ppppa[k4][i]=x[i].read();
        }
        k4++;
        Serial.println(k4);//反馈：存了多少个点
      }
      mode=6;
      last_an[5] = millis();
    }
  }
  if(digitalRead(7) == 0)   //读取按键5信号，当按键5被按下，进入模式5
  {
    if (millis() - last_an[4] >= t_an)
    {
      mode=5;
      
      last_an[4] = millis();
    }
  }
  if (digitalRead(4) == 0)  //读取按键4信号，当按键4被按下，进入模式4
  {
    if (millis() - last_an[3] >= t_an)
    {
      mode=4;
      

      last_an[3] = millis();
    }
  }
  if (digitalRead(3) == 0)  //读取按键3信号，当按键3被按下，进入模式3，但是先就位
  {
    if (millis() - last_an[2] >= t_an)
    {
      mode=3;
      Serial.println(333333);//反馈(进入模式3)
      k3=0;
      for(int i=0;i<=2;i++)//发令前往录制时的初始点
      {
        aa[i]=ppa[1][i];
      }
      back(aa,3);
      set_t(t,3,aa,3,last,3,x,3);
      last_record=millis()+3000;//小巧思：提供就位时间
      last_an[2] = millis();
    }
  }
  if (digitalRead(2) == 0)  //读取按键2信号，当按键2被按下，进入模式2或退出模式2
  {
    if (millis() - last_an[1] >= t_an)
    {
      if(mode!=2)
      {
        mode=2;
        Serial.println(222222222);//反馈(进入模式2)
        k2=0;//清零记录点计数
        last_record=millis();
      }
      else//如果已在模式2，退出
      {
        mode=4;
        Serial.println(444444);//反馈(进入模式4)
      }
      last_an[1] = millis();
    }
  }
  if (digitalRead(5) == 0)  //读取按键1信号，当按键1被按下
  {
    if (millis() - last_an[0] >= t_an)
    {
      Serial.println(111111);//反馈（已按按键1）
      if (k1 < 6)  //记录电机角度（六次）
      {
        if(k1>=0&&mode==1)
        {
          for (int i = 0; i <= 2; i++) 
          {
            pa[k1 + 1][i] =x[i].read() ;//不用aa[i]
          }
          k1++;
        }
        mode=1;
        Serial.println(k1);//反馈（记录点个数）
      } 
      else  //激活动作再现
      {
        mode=1;
        playback = 1;
        
      }
      
      last_an[0] = millis();
    }
  }

 
  if(mode==4 )//仅在刚进入模式4时执行一次：改变目标角
  {
    if( aa[0]!=pa[0][0]||aa[1]!=pa[0][1]||aa[2]!=pa[0][2])
    {
      for(int i=0;i<=2;i++)
      {
        aa[i]=pa[0][i];
      }
      back(aa,3);
      set_t(t,3,aa,3,last,3,x,3);
    }
  }
  else if(mode==2)//当进入模式2，按周期用pa[][3]来存舵机角
  {
    if(millis()-last_record>=t_record)
    {
      if(k2<100)//上限一百个点
      {
        for (int i = 0; i <= 2; i++) 
        {
          ppa[k2+1][i] =x[i].read();//读取机械臂姿态/（不用aa[i]）
        }
        last_record=millis();
        k2++;
      }
      else //录完自动退出
      {
        mode=4;
      Serial.println(444444);//反馈(进入模式4)
      }
    }
  }
  else if(mode==3)//当进入模式3，按周期改变目标角
  {
    if(millis()-last_record>=t_record)
    {
      if(k3<=k2)
      {
        for (int i = 0; i <= 2; i++) 
        {
          aa[i]=ppa[k3][i] ;//不要写k2！
          back(aa,3);
          set_t(t,3,aa,3,last,3,x,3);
        }
        last_record=millis();
        k3++;
      }
      else//播完自动退出
      {
        mode=4;
        Serial.println(444444);//反馈(进入模式4)
      }
    }
  }

  if(mode==1)//当进入模式1且激活复现，按周期执行动作
  {
    if (playback == 1)  //当动作复现被激活,按周期改变目标角
    {
       if (millis() - last_move >= t_move)
      {
        Serial.println(step_move);//反馈（执行计数）
        move_(aa,4,&step_move,&playback);
        back(aa,3);
        set_t(t,3,aa,3,last,3,x,3);
        last_move=millis();
      }
    }
  }
  else 
  {
    playback=0;
    step_move=1;
  }
  
  for (int i = 0; i <= 2; i++)  //读取摇杆信号
  {
    ua[i] = g(analogRead(pinA[i]));
    //ua[i] = analogRead(pinA[i]);
  }


  if ((abs(ua[0] - aa[0]) > 2 || abs(ua[1] - aa[1]) > 2 || abs(ua[2] - aa[2]) > 2) && ((playback==0&&mode==1)||mode==2||mode==5||mode==6))
  //当摇杆信号发生较大改变(模式1非复现期及模式2、5、6) 
  {
    for (int i = 0; i <= 2; i++)  //修改目标角
    {
      aa[i] = ua[i];
    }
    back(aa, 3);
    set_t(t, 3, aa, 3, last, 3, x, 3);
  }


  if(mode==5)//当进入模式5，接受串口控制
  {
    if(Serial.available()!=0)
    {
      char c1=Serial.read();
      if(c1=='o')
      {
        aa[3]=60;
      }
      else if(c1=='s')
      {
        aa[3]=120;
      }
      else if(c1=='h')
      {
        tt-=5;
      }
      else if(c1=='l')
      {
        tt+=5;
      }
      else if(c1=='x')
      {
        pppa[0]=Serial.parseInt();
        if(Serial.read()==',')
        {
          if(Serial.read()=='y')
          {
            pppa[1]=Serial.parseInt();
            if(Serial.read()==',')
            {
              if(Serial.read()=='z')
              {
                pppa[2]=Serial.parseInt();
                for(int i=0;i<=2;i++)
                {
                  aa[i]=pppa[i];
                  back(aa,3);
                  Serial.println(10.0*aa[0]/180+2.5);
                  Serial.println(10.0*aa[1]/180+2.5);
                  Serial.println(10.0*aa[2]/180+2.5);
                  set_t(t,3,aa,3,last,3,x,3);
                }
              }
            }
          }
        }
      }
      
    }
  }


  if(mode==7)//当进入模式7
  {
    if(millis()-last_push>=t_push)
    {
      if(pp[0]!=pppp[1][0]||pp[1]!=pppp[1][1]||pp[2]!=pppp[1][2])
      {
        for(int i=0;i<=2;i++)//改变目标坐标（加微量）
        {
          pp[i]=pp[i]+p[i];
        }
        h(pp,3,aa,3);  //改目标角
        back(aa,3);
        set_t(t,3,aa,3,last,3,x,3);
      }
    
      last_push=millis();
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

int g(int x)  //将摇杆信号换算为多舵机信号
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
      aa[i]=pa[int(*step_move*0.4+0.6)][i];
    }
  }
  else if(*step_move%5==2)//合爪
  {
    aa[3] = 120;
  }
  else if(*step_move%5==3)//移动至p2、p4、p6
  {
    for(int i=0;i<=2;i++)
    {
      aa[i]=pa[int(*step_move*0.4+0.8)][i];
    }
  }
  else if(*step_move%5==4)//开爪
  {
    aa[3] = 60;
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
  (*step_move)++;
}
// move_(aa,4,&step_move,&playback);

void h(double pp[],int npp,int aa[],int naa) //根据目标坐标直接改目标角 （模式7）
{
  double i=pp[0]*pp[0]+pp[1]*pp[1];
  double D=sqrt(pp[0]*pp[0]+pp[1]*pp[1])-r;
  //辅助计算
  
  double a[3]={0,0,0};//绝对角(中间量)
  a[0]=(atan(pp[2]/D)+acos((pp[2]*pp[2]+D*D+l1*l1-l2*l2)/(2*l1*sqrt(pp[2]*pp[2]+D*D))))/PI*180;
  a[1]=-1*acos((pp[2]*pp[2]+D*D-l1*l1-l2*l2)/(2*l1*l2))/PI*180+a[0];
  a[2]=asin(pp[1]/sqrt(i))/PI*180;      

  aa[0]=a[0]*-1+179;
  aa[1]=a[1]+98;
  aa[2]=a[2]+93;
}          
//h(pp,3,aa,3);               


void hh(double pppp[],int npppp,int ppppa[],int nppppa)//根据姿态直接改对应坐标（模式7）
{
  double a[3]={0,0,0};//绝对角(中间量)
  a[0]=179-ppppa[0];
  a[1]=ppppa[1]-98;
  a[2]=ppppa[2]-93;

  pppp[0]=cos(a[2])*(l1*cos(a[0])+l2*cos(a[1])+r);
  pppp[1]=sin(a[2])*(l1*cos(a[0])+l2*cos(a[1])+r);
  pppp[2]=l1*sin(a[0])+l2*sin(a[1]);
}

void hhh(double p[],int np)//根据两点姿态ppppa直接算位移微量p（模式7）
{
  hh(pppp[0],3,ppppa[0],3);
  hh(pppp[1],3,ppppa[1],3);
  double ppp[3]={0,0,0};//求位移量
  for(int i=0;i<=2;i++)
  {
    ppp[i]=pppp[1][i]-pppp[0][i];
  }
  double long_ppp=ppp[0];//确定最长的位移分量
  for(int i=1;i<=2;i++)
  {
    if(abs(ppp[i])>=abs(long_ppp))//加绝对值才严谨
    long_ppp=ppp[i];
  }
  for(int i=0;i<=2;i++)//求位移微量
  {
    p[i]=ppp[i]/long_ppp;
  }

}
//hhh(p,3);
