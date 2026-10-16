#include <Servo.h>
int tt =30;                               //运动周期
#define d 1                               //步长
int aa[4] = { 80, 98, 93, 60 };           //目标角
unsigned long last[4] = { 0, 0, 0, 0 };   //上次执行时间点
Servo x[4];                               //三个电机
int t[4] = { tt, tt, tt, 2 };             //实际运动周期
int ua[3];                                //由摇杆电压值换算得到的角度值
int pinA[2] = {A1, A3};                   //摇杆信号接收管脚
int t_an = 300;                                       //按键响应周期
unsigned long last_an[7] = {0,0,0,0,0,0,0};           //按键上次响应时间
int t_joystick = 100;                                 //摇杆响应周期
unsigned long last_joystick = 0;                      //摇杆上次响应时间
int pa[7][3] = { { 80, 98, 93 } };    //第一个表示初始位置的姿态，后六个表示记录下来的姿态（任务3模式1）
int ppa[101][3] = { { 80, 98, 93 } }; //第一个表示初始位置的姿态，后面表示记录下来的姿态（任务3模式2）
int k1 = 0;                            //任务3模式1记录点数
int k2 = 0;                            //任务3模式2录制点数
int k3 = 0;                            //任务3模式3播放点数
int k4 = 0;                            //任务5模式1记录点数
int k5 = 0;                            //任务5模式2绘制点数
int t_record =100;                  //姿态录制（动作命令发出）周期（任务3模式2、3）
unsigned long last_record = 0;        //姿态上次录制（动作命令上次发出）时间点（任务3模式2、3）
int t_perform = 4000;                  //动作命令发出周期（任务3模式1）
unsigned long last_perform = 0;        //动作命令上次发出时间点（任务3模式1）
int step_perform = 1;                             //发令编号（任务3模式1）
bool will_perform = 0;                              //是否进入执行过程（任务3模式1）
int task=3;          //任务
int mode=0;          //模式  
char tip='f';        //暂存串口输入（来自按键板）              //int pppa[3]={0,0,0};//（任务1模式2）（停用）
int ppppa[5][3]={0,0,0};//记录描点姿态(任务5模式1)
int t_push = 100;                  //动作命令发出周期（任务5模式2）
unsigned long last_push = 0;        //动作命令上次发出时间点（任务5模式2）
#define l1 8
#define l2 8
#define r 1.45
double pp[3]={0,0,0};//目标位置坐标（任务5模式2）
double a[3]={0,0,0};//绝对角（中间量)(任务5模式2)
double pppp[5][3]={0,0,0};//记录五点姿态（ppppa）对应的坐标（任务5模式2）
double ppp[3]={0,0,0};//记录两点间的位移量（中间量）（任务5模式2）
double long_ppp=0;//最长位移分量（中间量）（任务5模式2）
double p[3]= {0,0,0};//画线阶段位移微量（任务5模式2）
unsigned long key=0;//（摇杆双击计时）

void setup() 
{
  Serial.begin(9600);
  x[0].attach(7);
  x[1].attach(8);
  x[2].attach(9);
  x[3].attach(6);
  
  pinMode(5,INPUT_PULLUP);//按键左
  pinMode(4,INPUT_PULLUP);//按键右
  

}

void loop()
{
  if(Serial.available())//接收来自按键板的信息
  {
    tip=Serial.read();
  }

  if(tip=='e')//切换任务
  {
    if(task<5)task+=2;
    else task=1;
    mode= 0;
    Serial.print("enter task");Serial.print(task);Serial.println("    choose mode to enter");
    aa[0]=89;aa[1]=89;aa[2]=93;
    if(task==5) aa[3]=150;else aa[3]=60;
    k1=0;k2=0;k4=0;
    tip='f';
  }



  if(task==1)//任务1
  {
    if(mode==1)//摇杆控制
    {}
    else if(mode==2)//串口控制//（禁用）
    {
      if(Serial.available()!=0)
      {
        char c1=Serial.read();
        if(c1=='o')//开爪
        {
          aa[3]=60;
          Serial.println("gripper opened");
        }
        else if(c1=='s')//合爪
        {
          aa[3]=150;
          Serial.println("gripper closed");
        }
        else if(c1=='h')//加速
        {
          tt-=5;
          Serial.println("speed higher");
        }
        else if(c1=='l')//减速
        {
          tt+=5;
          Serial.println("speed lower");
        }
        else if(c1=='x')//串口三角度指令
        {
          for(int i=0;i<=2;i++)
          {
            aa[i]=Serial.parseInt();
            set_t(t,3,aa,3,last,3,x,3);
          }
                  
                      
                   
        
          Serial.print("start moving to ");Serial.print(" x");Serial.print(aa[0]);Serial.print(" y");Serial.print(aa[1]);Serial.print(" z");Serial.println(aa[2]);
          Serial.print("return dc: ");Serial.print(" x");Serial.print(10.0*aa[0]/180+2.5);Serial.print("%");Serial.print(" y");Serial.print(10.0*aa[1]/180+2.5);Serial.print("%");Serial.print(" z");Serial.print(10.0*aa[2]/180+2.5);Serial.println("%");

         
        }
        else
        {
          Serial.println("input fault");
        }
      }
    }



    if(tip=='a')//读取按键1信号，当按键1被按下
    {     
      Serial.println("press 1");//反馈（已按按键1）
      if(mode!=1)
      {
        Serial.println("enter mode1");
        mode=1;
      }
        
      tip ='f';
             
    }


    if(tip=='b')//读取按键2信号，当按键2被按下
    {
      Serial.println("press 2");//反馈（已按按键2）
      if(mode!=2)
      {
        Serial.println("enter mode2");
        mode=2;
      }
      
      tip='f';
        
      
    }

  }



  if(task==3)//任务3
  {
    if (tip=='a')  //读取按键1信号，当按键1被按下
    {
      
      Serial.println("press 1");//反馈（已按按键1）

      if(mode==1)//如果已在模式1
      {           
        if (k1 < 6)  //如果没有存够
        {
          for (int i = 0; i <= 2; i++) 
          {
            pa[k1 + 1][i] =x[i].read() ;
          }
          k1++;
          Serial.print("have logged:   ");Serial.println(k1);
          if(k1<6)Serial.println("press again to log") ;
          else Serial.println("press again to perform");
          
        }
        else//如果已经存够
        {
          Serial.println("start performing");
          mode=1;
          will_perform = 1;       
        }
      }
      else//如果刚进模式1
      {
        Serial.println("enter mode1");
        mode=1;
        Serial.print("have logged:   ");Serial.println(k1);
        if(k1<6)Serial.println("press again to log") ;
        else Serial.println("press again to perform");
      }      
        
        
                      
      tip='f';
      
    }
    if (tip=='b')  //读取按键2信号，当按键2被按下，进入模式2或退出模式2
    {
      
      Serial.println("press 2");//反馈
      if(mode!=2)
      {
        mode=2;
        Serial.println("enter mode2");//反馈
        k2=0;//清零记录点计数
        Serial.println("start recording");//反馈
        last_record=millis();
      }
      else//如果已在模式2，停止记录
      {
        mode=0;
        Serial.println("complete recording");//反馈
      }
      tip='f';
      
    }
    if (tip=='c')  //读取按键3信号，当按键3被按下，进入模式3，但是先就位
    {
      
      mode=3;
      Serial.println("press 3");//反馈

      if(k2==0)
      {
        mode=0;
        Serial.println("have not recorded");//反馈
      }
      else
      {
        Serial.println("enter mode3");//反馈
        Serial.println("start playing");//反馈
        k3=1;
        for(int i=0;i<=2;i++)//发令前往录制时的初始点
        {
          aa[i]=ppa[1][i];
        }
        set_t(t,3,aa,3,last,3,x,3);
        last_record=millis()+4000;//小巧思：提供就位时间

      }
            
      tip='f';
      
    }
    if (tip=='d')  //读取按键4信号，当按键4被按下，进入模式4
    {
      
      Serial.println("press 4");//反馈(进入模式4)
      Serial.println("enter 4");
      mode=4;
      tip='f';
      
    }


    if(mode==1)//当进入模式1且激活复现，按周期执行动作
    {
      if (will_perform == 1)  //当动作执行被激活,按周期发令
      {        
        if (millis() - last_perform >= t_perform)
        {
          perform();
          set_t(t,3,aa,3,last,3,x,3);
          last_perform=millis();
        }
      }
    }
    else //退出模式一将保留计数点，但是步数、激活状态清零
    {
      will_perform=0;
      step_perform=1;
    }
    if(mode==2)//当进入模式2，按周期用pa[][3]来存舵机角
    {
      if(millis()-last_record>=t_record)
      {
        if(k2<100)//上限一百个点
        {
          for (int i = 0; i <= 2; i++) 
          {
            ppa[k2+1][i] =x[i].read();//读取机械臂姿态
          }
          last_record=millis();
          k2++;
        }
        else //录够十秒自动停止
        {
          mode=0;
          Serial.println("have recorded for 10s,stop recording");
        }
      }
    }
    if(mode==3)//当进入模式3，按周期改变目标角
    {
      if(millis()-last_record>=t_record)
      {
        if(k3<=k2)
        {
          for (int i = 0; i <= 2; i++) 
          {
            aa[i]=ppa[k3][i] ;
            set_t(t,3,aa,3,last,3,x,3);
          }
          last_record=millis();
          k3++;
        }
        else//播完自动退出
        {
          mode=0;
          Serial.println("complete playing");//反馈
        }
      }
    }
    if(mode==4 )//仅在刚进入模式4时执行一次：改变目标角
    {
      if(aa[0]!=pa[0][0]||aa[1]!=pa[0][1]||aa[2]!=pa[0][2])
      {
        for(int i=0;i<=2;i++)
        {
          aa[i]=pa[0][i];
        }
        aa[3]=60;//00000000
        set_t(t,3,aa,3,last,3,x,3);
      }
    }







  }


  if(task==5)//任务5
  {
    if(tip=='a')//读取按键1信号，当按键1被按下，（未在模式1：进入模式1；已在模式1：记录一次）
    {
      
      Serial.println("press 1");//反馈：按键1已响应
      if(mode==1&&k4<5)//如果已在模式1且未记录满
      {
        for(int i=0;i<=2;i++)//记录
        {
          ppppa[k4][i]=x[i].read();
        }
        k4++;
        Serial.print("have logged:    "); Serial.println(k4);        
      }
      else if(mode==1&&k4==5)//如果已在模式1且已存满5个点
      {
        k4=0;
        Serial.println("all point has been deleted");
      }
      else
      {
        mode=1;
        Serial.println("enter mode1");
        Serial.print("have logged:    ");Serial.println(k4);        
      }
      if(k4<=1)
      Serial.println("press again to log");     
      else if(k4<5)
      Serial.println("press again to log,or enter mode2 to paint");      
      else 
      Serial.println("press again to delete,or enter mode2 to paint");     
      tip='f';
    }

    if (tip=='b')  //读取按键2信号，当按键2被按下，若已有至少两个记录：进入模式2
    {     
      Serial.println("press 2");//反馈：按键2已响应
      if(mode==2)//如果已在模式2
      {
        if(k4<=1)
        Serial.println("failed to start");
        else
        {
          Serial.println("start painting");
          k5=0;
          last_push=millis()+5000;//给足时间就位
          for(int i=0;i<=2;i++)//就位
          {
            aa[i]=ppppa[0][i];
          }
          set_t(t,3,aa,3,last,3,x,3);
          hh();//确定位移微量
          for(int i=0;i<=2;i++)//锚定初始目标坐标
          {
            pp[i]=pppp[0][i];
          }

        }
        tip='f';
      }
      else //如果未在模式2
      {
        mode=2;
        Serial.println("enter mode2");
        k5=0;
        aa[0]=89;aa[1]=89;aa[2]=93;
        Serial.print("have logged:    ");Serial.println(k4);
        if(k4<=1)
        Serial.println("enter mode1 to log ");
        else 
        Serial.println("press again to start painting");
        
      }

      
    }
  
    if(tip=='c')//读取按键3信号，启动暂停/继续功能
    { 
      if(mode==2)
      {
        mode=3;
        Serial.println("pause");       
      }
      else if(mode==3)
      {
        mode=2;
        Serial.println("continue");
      }     
      tip="f";
    }

    
    if(tip=='d')//读取按键4信号，启动取消功能
    {
      k4=0;
      mode=1;
      Serial.println("cancel");
    }

    if(mode==2)//当进入模式2
    {     
      if(millis()-last_push>=t_push)
      {
        if(abs(pp[0]-pppp[k5][0])<=0.3&&abs(pp[1]-pppp[k5][1])<=0.3&&abs(pp[2]-pppp[k5][2])<=0.3)//抵达
        {
          k5++;
          Serial.print("have reached: ");Serial.println(k5);//反馈（已抵达多少个点）
          if(k5<k4)//未抵达最后一个记录点
          {          
            hh();//求微量
          }
          else if(k5==k4)//抵达最后一个记录点
          {
            Serial.println("complete painting");//反馈
            mode=0;//停
          }
        }
        else//未抵达
        {
          Serial.print(".");//反馈
          for(int i=0;i<=2;i++)//改变目标坐标（加微量）
          {
            pp[i]=pp[i]+p[i];
          }
          h();  
          set_t(t,3,aa,3,last,3,x,3);
        }
      
        last_push=millis();
      }
    }


  }


  if  ((will_perform==0&&task==3&&mode==1)||(task==3&&mode==2)||(task==1&&mode==1)||(task==5&&mode==1))
  //当摇杆被启用
  {


    if(digitalRead(5)==0)//底盘左转
    {
      if(millis()-last_an[5]>=200)
      {
        aa[2]+=2;
        //Serial.println("left");
        key=millis();
        last_an[5]=millis();
      }        
      
    }
    if(digitalRead(4)==0)//底盘右转
    {
      if(millis()-last_an[6]>=200)
      {
        aa[2]-=2;
        //Serial.println("right");
        if(millis()-key<=100)
        {
          aa[3]=180-aa[3];  
          //Serial.println("gripper");
          key=0;
        }        
        last_an[6]=millis();
      }        
      
    }

    

    if(millis()-last_joystick>=t_joystick)//摇杆控肩肘
    {

      
      ua[0]=analogRead(A1);
      ua[1]=analogRead(A3);
      
      for(int i=0;i<=1;i++)
      {
        if(ua[i]>=600)
        {
          aa[i]+=3;
          Serial.print(i);  Serial.print(" ");  Serial.println(aa[i]);
        }
        else if(ua[i]<=450)
        {
          aa[i]-=3;
          Serial.print(i);  Serial.print("  ");  Serial.println(aa[i]);
        }
      }
      last_joystick=millis();
      set_t(t, 3, aa, 3, last, 3, x, 3);//set运动周期
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

void perform()//依靠指令发出计数（step_perform），修改目标角aa[]并管理will_perform、step_perform值（模式1复现期）
{
  if(step_perform%5==1)//当step_perform=1、6、11，移动至p1、p3、p5（取物）
  {
    for(int i=0;i<=2;i++)
    {
      aa[i]=pa[int(step_perform*0.4+0.6)][i];
    }
    Serial.println("fetch");//反馈
  }
  else if(step_perform%5==2)//当step_perform=2、7、12，合爪
  {
    aa[3] = 120;
    Serial.println("close gripper");//反馈
  }
  else if(step_perform%5==3)//当step_perform=3、8、13，移动至p2、p4、p6（运送）
  {
    for(int i=0;i<=2;i++)
    {
      aa[i]=pa[int(step_perform*0.4+0.8)][i];
    }
    Serial.println("carry");//反馈
  }
  else if(step_perform%5==4)//当step_perform=4、9、14，开爪
  {
    aa[3] = 60;
    Serial.println("open gripper");//反馈
  }
  else if(step_perform%5==0)//当step_perform=5、10、15，复位
  {
    for(int i=0;i<=2;i++)
    {
      aa[i]=pa[0][i];
      will_perform=0;
      Serial.println("press again to perform");
    }
    Serial.println("reset");//反馈
  }
  if(step_perform==15)
  {
    step_perform=0;
  }
  (step_perform)++;
}
// perform();

void h() //根据目标坐标pp直接改目标角aa （模式7）
{
  double i=pp[0]*pp[0]+pp[1]*pp[1];//辅助计算
  double D=sqrt(pp[0]*pp[0]+pp[1]*pp[1])-r;
   
  a[0]=(atan(pp[2]/D)+acos((pp[2]*pp[2]+D*D+l1*l1-l2*l2)/(2*l1*sqrt(pp[2]*pp[2]+D*D))))/PI*180;//算目标坐标对应的绝对角(中间量)
  a[1]=-1*acos((pp[2]*pp[2]+D*D-l1*l1-l2*l2)/(2*l1*l2))/PI*180+a[0];
  a[2]=asin(pp[1]/sqrt(i))/PI*180;      

  aa[0]=a[0]*-1+178;//绝对角转舵机角
  aa[1]=a[1]+89;
  aa[2]=a[2]+93;
}          
//h();               


void hh()//根据两点姿态ppppa得对应坐标pppp再算位移微量p（模式7）
{
  for(int i=k5-1;i<=k5;i++)//根据两个姿态ppppa直接改对应坐标pppp
  {
    a[0]=178-ppppa[i][0];//舵机角转绝对角(中间量)
    a[1]=ppppa[i][1]-89;
    a[2]=ppppa[i][2]-93;

    pppp[i][0]=cos(a[2]/180*PI)*(l1*cos(a[0]/180*PI)+l2*cos(a[1]/180*PI)+r);
    pppp[i][1]=sin(a[2]/180*PI)*(l1*cos(a[0]/180*PI)+l2*cos(a[1]/180*PI)+r);
    pppp[i][2]=l1*sin(a[0]/180*PI)+l2*sin(a[1]/180*PI);
  }
  

  if(k5>0)
  {
    for(int i=0;i<=2;i++)  //求位移量ppp(绝对值)（中间量）
    {
      ppp[i]=abs(pppp[k5][i]-pppp[k5-1][i]);
    }
    
    long_ppp=ppp[0]; 
    for(int i=1;i<=2;i++)//确定最长的位移分量long_ppp（中间量）
    {
      if(ppp[i]>=long_ppp)
      long_ppp=ppp[i];
    }
    for(int i=0;i<=2;i++)//求位移微量p
    {
      p[i]=ppp[i]/long_ppp/10;
      if(pppp[k5][i]<pppp[k5-1][i])
      {
        p[i]*=-1;
      }
      else if(pppp[k5][i]==pppp[k5-1][i])
      {
        p[i]=0;
      }
    }
  }

}
//hh();

//此为双板对话版本，不过按键板将暂时用自制简陋板替代
//按键板只发不接，上位机只接不发
//计划用于任务3、5的视频演示及三面答辩（禁用任务1模式2）
//但是任务5还没做完，我先试试能不能对话