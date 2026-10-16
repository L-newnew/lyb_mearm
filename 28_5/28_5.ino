#include <Servo.h>
int t_drive_base =30;                               //最快运动周期
#define step_drive 1                               //步长
int angle_target[4] = { 89, 89, 93, 60 };           //目标角
unsigned long last_drive[4] = { 0, 0, 0, 0 };   //上次执行时间点
Servo servo[4];                               //三个电机
int t_drive_actual[4] = { t_drive_base, t_drive_base, t_drive_base, 2 };             //实际运动周期
int ua[3];                                //由摇杆电压值换算得到的角度值
int pinA[2] = {A1, A3};                   //摇杆信号接收管脚
int t_an = 500;                                       //按键响应周期
unsigned long last_an[2] = {0,0};           //按键上次响应时间
int t_joystick = 100;                                 //摇杆响应周期
unsigned long last_joystick = 0;                      //摇杆上次响应时间
int task3_poses_logged[7][3] = { { 89, 89, 93 } };    //第一个表示初始位置的姿态，后六个表示记录下来的姿态（任务3模式1）
int task3_poses_recorded[101][3] = { { 80, 98, 93 } }; //第一个表示初始位置的姿态，后面表示记录下来的姿态（任务3模式2）
int task3_log_record = 0;                            //任务3模式1记录点数
int task3_count_record = 0;                            //任务3模式2录制点数
int task3_play_record = 0;                            //任务3模式3播放点数
int task5_count_log = 0;                            //任务5模式1记录点数
int task5_count_paint = 0;                            //任务5模式2绘制点数
int t_record_or_play =100;                  //姿态录制（动作命令发出）周期（任务3模式2、3）
unsigned long last_record_or_play = 0;        //姿态上次录制（动作命令上次发出）时间点（任务3模式2、3）
int t_perform = 4000;                  //动作命令发出周期（任务3模式1）
unsigned long last_perform = 0;        //动作命令上次发出时间点（任务3模式1）
int step_perform = 1;                             //发令编号（任务3模式1）
bool will_perform = 0;                              //是否进入执行过程（任务3模式1）
int task=3;          //任务
int mode=0;          //模式  
char tip='f';        //暂存串口输入（来自按键板）              //int coord_target[3]={0,0,0};//（任务1模式2）（停用）
int task5_poses_logged[5][3]={0,0,0};//记录描点姿态(任务5模式1)
int t_paint = 100;                  //动作命令发出周期（任务5模式2）
unsigned long last_paint = 0;        //动作命令上次发出时间点（任务5模式2）
#define l1 8
#define l2 8
#define r 1.45
double coord_target[3]={0,0,0};//目标位置坐标（任务5模式2）
double angle_abs[3]={0,0,0};//绝对角（中间量)(任务5模式2)
double task5_coords_record[5][3]={0,0,0};//记录五点姿态（task5_poses_logged）对应的坐标（任务5模式2）
double coords_delta[3]={0,0,0};//记录两点间的位移量（中间量）（任务5模式2）
double coords_delta_longest=0;//最长位移分量（中间量）（任务5模式2）
double paint_step[3]= {0,0,0};//画线阶段位移微量（任务5模式2）
unsigned long time_doublepress=0;//（摇杆双击计时）
unsigned long last_doublepress;


void setup() 
{
  Serial.begin(9600);
  servo[0].attach(7);
  servo[1].attach(8);
  servo[2].attach(9);
  servo[3].attach(6);
  
  pinMode(5,INPUT_PULLUP);//按键左
  pinMode(4,INPUT_PULLUP);//按键右
  

}

void loop()
{
  if(Serial.available())//接收来自按键板的信息
  {
    if(task!=1||mode!=2)
    {
        tip=Serial.read();
    }
  }


  if(tip=='e')//切换任务
  {
    if(task<5)task+=2;
    else task=1;
    mode= 0;
    Serial.print("enter task");Serial.print(task);Serial.println("    choose mode to enter");
    angle_target[0]=89;angle_target[1]=89;angle_target[2]=93;
    if(task==5) angle_target[3]=150;else angle_target[3]=60;
    task3_log_record=0;task3_count_record=0;task5_count_log=0;
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
          angle_target[3]=60;
          Serial.println("gripper opened");
        }
        else if(c1=='s')//合爪
        {
          angle_target[3]=150;
          Serial.println("gripper closed");
        }
        else if(c1=='h')//加速
        {
          t_drive_base-=5;
          Serial.println("speed higher");
        }
        else if(c1=='l')//减速
        {
          t_drive_base+=5;
          Serial.println("speed lower");
        }
        else if(c1=='x')//串口三角度指令
        {
          for(int i=0;i<=2;i++)
          {
            angle_target[i]=Serial.parseInt();
            t_set();
          }
                  
                      
                   
        
          Serial.print("start moving to ");Serial.print(" x");Serial.print(angle_target[0]);Serial.print(" y");Serial.print(angle_target[1]);Serial.print(" z");Serial.println(angle_target[2]);
          Serial.print("return dc: ");Serial.print(" x");Serial.print(10.0*angle_target[0]/180+2.5);Serial.print("%");Serial.print(" y");Serial.print(10.0*angle_target[1]/180+2.5);Serial.print("%");Serial.print(" z");Serial.print(10.0*angle_target[2]/180+2.5);Serial.println("%");

         
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
        if (task3_log_record < 6)  //如果没有存够
        {
          for (int i = 0; i <= 2; i++) 
          {
            task3_poses_logged[task3_log_record + 1][i] =servo[i].read() ;
          }
          task3_log_record++;
          Serial.print("have logged:   ");Serial.println(task3_log_record);
          if(task3_log_record<6)Serial.println("press again to log") ;
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
        Serial.print("have logged:   ");Serial.println(task3_log_record);
        if(task3_log_record<6)Serial.println("press again to log") ;
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
        task3_count_record=0;//清零记录点计数
        Serial.println("start recording");//反馈
        last_record_or_play=millis();
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

      if(task3_count_record==0)
      {
        mode=0;
        Serial.println("have not recorded");//反馈
      }
      else
      {
        Serial.println("enter mode3");//反馈
        Serial.println("start playing");//反馈
        task3_play_record=1;
        for(int i=0;i<=2;i++)//发令前往录制时的初始点
        {
          angle_target[i]=task3_poses_recorded[1][i];
        }
        t_set();
        last_record_or_play=millis()+4000;//小巧思：提供就位时间

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
          t_set();
          last_perform=millis();
        }
      }
    }
    else //退出模式一将保留计数点，但是步数、激活状态清零
    {
      will_perform=0;
      step_perform=1;
    }
    if(mode==2)//当进入模式2，按周期用task3_poses_logged[][3]来存舵机角
    {
      if(millis()-last_record_or_play>=t_record_or_play)
      {
        if(task3_count_record<100)//上限一百个点
        {
          for (int i = 0; i <= 2; i++) 
          {
            task3_poses_recorded[task3_count_record+1][i] =servo[i].read();//读取机械臂姿态
          }
          last_record_or_play=millis();
          task3_count_record++;
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
      if(millis()-last_record_or_play>=t_record_or_play)
      {
        if(task3_play_record<=task3_count_record)
        {
          for (int i = 0; i <= 2; i++) 
          {
            angle_target[i]=task3_poses_recorded[task3_play_record][i] ;
            t_set();
          }
          last_record_or_play=millis();
          task3_play_record++;
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
      if(angle_target[0]!=task3_poses_logged[0][0]||angle_target[1]!=task3_poses_logged[0][1]||angle_target[2]!=task3_poses_logged[0][2])
      {
        for(int i=0;i<=2;i++)
        {
          angle_target[i]=task3_poses_logged[0][i];
        }
        angle_target[3]=60;//00000000
        t_set();
      }
    }

  }


  if(task==5)//任务5
  {
    if(tip=='a')//读取按键1信号，当按键1被按下，（未在模式1：进入模式1；已在模式1：记录一次）
    {
      
      Serial.println("press 1");//反馈：按键1已响应
      if(mode==1&&task5_count_log<5)//如果已在模式1且未记录满
      {
        for(int i=0;i<=2;i++)//记录
        {
          task5_poses_logged[task5_count_log][i]=servo[i].read();
        }
        task5_count_log++;
        angle_target[0]=89;angle_target[1]=89;angle_target[2]=93;angle_target[3]=150;
        Serial.print("have logged:    "); Serial.println(task5_count_log);        
      }
      else if(mode==1&&task5_count_log==5)//如果已在模式1且已存满5个点
      {
        task5_count_log=0;
        Serial.println("all point has been deleted");//反馈：存了多少个点
      }
      else
      {
        mode=1;
        Serial.println("enter mode1");
        Serial.print("have logged:    ");Serial.println(task5_count_log);        
      }
      if(task5_count_log<=1)
      Serial.println("press again to log");     
      else if(task5_count_log<5)
      Serial.println("press again to log,or enter mode2 to paint");      
      else 
      Serial.println("press again to delete,or enter mode2 to paint");     
      tip='f';
    }

    if (tip=='b')  //读取按键2信号，当按键2被按下，若已有至少两个记录：进入模式2
    {     
      Serial.println("press 2");//反馈：按键2已响应
      
      if(task5_count_log<=1)
      Serial.println("failed to enter");
      else
      {
        mode=2;
        Serial.println("enter mode2");
        Serial.println("start painting");
        task5_count_paint=0;
        last_paint=millis()+5000;//给足时间就位
        for(int i=0;i<=2;i++)//就位
        {
          angle_target[i]=task5_poses_logged[0][i];
        }
        t_set();
        hh();//确定位移微量
        for(int i=0;i<=2;i++)//锚定初始目标坐标
        {
          coord_target[i]=task5_coords_record[0][i];
        }

      }
      tip='f';
      
      

      
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
      tip='f';
    }

    
    if(tip=='d')//读取按键4信号，启动取消功能
    {
      task5_count_log=0;
      mode=1;
      Serial.println("cancel");
      tip='f';
    }

    if(mode==2)//当进入模式2
    {     
      if(millis()-last_paint>=t_paint)
      {
        if(abs(coord_target[0]-task5_coords_record[task5_count_paint][0])<=0.3&&abs(coord_target[1]-task5_coords_record[task5_count_paint][1])<=0.3&&abs(coord_target[2]-task5_coords_record[task5_count_paint][2])<=0.3)//抵达
        {
          task5_count_paint++;
          Serial.print("have reached: ");Serial.println(task5_count_paint);//反馈（已抵达多少个点）
          if(task5_count_paint<task5_count_log)//未抵达最后一个记录点
          {          
            hh();//求微量
          }
          else if(task5_count_paint==task5_count_log)//抵达最后一个记录点
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
            coord_target[i]=coord_target[i]+paint_step[i];
          }
          h();  
          t_set();
        }
      
        last_paint=millis();
      }
    }


  }


  if  ((will_perform==0&&task==3&&mode==1)||(task==3&&mode==2)||(task==1&&mode==1)||(task==5&&mode==1))
  //当摇杆被启用
  {


    if(digitalRead(5)==0)//底盘左转
    {
      if(millis()-last_an[0]>=200)
      {
        if(angle_target[2]<=170)
        {
          angle_target[2]+=2;
        }            
        //Serial.println("left");
        time_doublepress=millis();
        last_an[0]=millis();
      }        
      
    }
    if(digitalRead(4)==0)//底盘右转
    {
      if(millis()-last_an[1]>=200)
      {
        if(angle_target[2]>=10)
        {
          angle_target[2]-=2;
        }
        //Serial.println("right");

        if(millis()-time_doublepress<=100)
        {
          if(millis()-last_doublepress>=500)
          {
            angle_target[3]=180-angle_target[3];  
            Serial.println(angle_target[3]);
            time_doublepress=0;
            last_doublepress=millis();
          }
          
        }        
        last_an[1]=millis();
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
          if(angle_target[i]<=170)
          {
            angle_target[i]+=3;
            Serial.print(i);  Serial.print(" ");  Serial.println(angle_target[i]);
          }
        }
        else if(ua[i]<=450)
        {
          if(angle_target[i]>=10)
          {
            angle_target[i]-=3;
            Serial.print(i);  Serial.print("  ");  Serial.println(angle_target[i]);
          }
          
        }
      }
      last_joystick=millis();
      t_set();//set运动周期
    }
    
  }

  servo_drive();  //按实际周期、目标角驱动各个电机

}


void servo_drive()  //令所有电机以设定周期运动到设定终点
{
  for (int i = 0; i <= 3; i++)
  {
    int k, angle_current = servo[i].read();
    if (angle_target[i] != angle_current && millis() - last_drive[i] >= t_drive_actual[i]) 
    {
      if (angle_target[i] > angle_current) k = 1;
      else if (angle_target[i] < angle_current) k = -1;

      if (abs(angle_target[i] - angle_current) < step_drive) angle_current = angle_target[i];
      else angle_current = angle_current + step_drive * k;

      servo[i].write(angle_current);
      last_drive[i] = millis();
    }
  }
}




void t_set() //算路程、算实际周期、改实际周期   
 //函数调用前提：触发三关节目标角修改
{
  int ss = 0;           //最长路程
  int s[3]={0,0,0};      //路程
  for (int i = 0; i <= 2; i++)//算路程 
  {
    s[i] = abs(servo[i].read() - angle_target[i]);
    if (s[i] >= ss) ss = s[i];
  }

  for (int i = 0; i <= 2; i++)  //算实际周期,改实际周期
  {
    if (s[i] != 0) t_drive_actual[i] = ss * t_drive_base / s[i];
    last_drive[i] = millis();
  }
}

void perform()//依靠指令发出计数（step_perform），修改目标角angle_target[]并管理will_perform、step_perform值（模式1复现期）
{
  if(step_perform%5==1)//当step_perform=1、6、11，移动至p1、p3、p5（取物）
  {
    for(int i=0;i<=2;i++)
    {
      angle_target[i]=task3_poses_logged[int(step_perform*0.4+0.6)][i];
    }
    Serial.println("fetch");//反馈
  }
  else if(step_perform%5==2)//当step_perform=2、7、12，合爪
  {
    angle_target[3] = 150;
    Serial.println("close gripper");//反馈
    
  }
  else if(step_perform%5==3)//当step_perform=3、8、13，移动至p2、p4、p6（运送）
  {
    for(int i=0;i<=2;i++)
    {
      angle_target[i]=task3_poses_logged[int(step_perform*0.4+0.8)][i];
    }
    Serial.println("carry");//反馈
  }
  else if(step_perform%5==4)//当step_perform=4、9、14，开爪
  {
    angle_target[3] = 60;
    Serial.println("open gripper");//反馈
  }
  else if(step_perform%5==0)//当step_perform=5、10、15，复位
  {
    for(int i=0;i<=2;i++)
    {
      angle_target[i]=task3_poses_logged[0][i];
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
  double i=coord_target[0]*coord_target[0]+coord_target[1]*coord_target[1];//辅助计算
  double D=sqrt(coord_target[0]*coord_target[0]+coord_target[1]*coord_target[1])-r;
   
  angle_abs[0]=(atan(coord_target[2]/D)+acos((coord_target[2]*coord_target[2]+D*D+l1*l1-l2*l2)/(2*l1*sqrt(coord_target[2]*coord_target[2]+D*D))))/PI*180;//算目标坐标对应的绝对角(中间量)
  angle_abs[1]=-1*acos((coord_target[2]*coord_target[2]+D*D-l1*l1-l2*l2)/(2*l1*l2))/PI*180+angle_abs[0];
  angle_abs[2]=asin(coord_target[1]/sqrt(i))/PI*180;      

  angle_target[0]=angle_abs[0]*-1+178;//绝对角转舵机角
  angle_target[1]=angle_abs[1]+89;
  angle_target[2]=angle_abs[2]+93;
}          
//h();               


void hh()//根据两点姿态task5_poses_logged得对应坐标task5_coords_record再算位移微量paint_step（模式7）
{
  for(int i=task5_count_paint-1;i<=task5_count_paint;i++)//根据两个姿态task5_poses_logged直接改对应坐标task5_coords_record
  {
    angle_abs[0]=178-task5_poses_logged[i][0];//舵机角转绝对角(中间量)
    angle_abs[1]=task5_poses_logged[i][1]-89;
    angle_abs[2]=task5_poses_logged[i][2]-93;

    task5_coords_record[i][0]=cos(angle_abs[2]/180*PI)*(l1*cos(angle_abs[0]/180*PI)+l2*cos(angle_abs[1]/180*PI)+r);
    task5_coords_record[i][1]=sin(angle_abs[2]/180*PI)*(l1*cos(angle_abs[0]/180*PI)+l2*cos(angle_abs[1]/180*PI)+r);
    task5_coords_record[i][2]=l1*sin(angle_abs[0]/180*PI)+l2*sin(angle_abs[1]/180*PI);
  }
  

  if(task5_count_paint>0)
  {
    for(int i=0;i<=2;i++)  //求位移量coords_delta(绝对值)（中间量）
    {
      coords_delta[i]=abs(task5_coords_record[task5_count_paint][i]-task5_coords_record[task5_count_paint-1][i]);
    }
    
    coords_delta_longest=coords_delta[0]; 
    for(int i=1;i<=2;i++)//确定最长的位移分量coords_delta_longest（中间量）
    {
      if(coords_delta[i]>=coords_delta_longest)
      coords_delta_longest=coords_delta[i];
    }
    for(int i=0;i<=2;i++)//求位移微量p
    {
      paint_step[i]=coords_delta[i]/coords_delta_longest/10;
      if(task5_coords_record[task5_count_paint][i]<task5_coords_record[task5_count_paint-1][i])
      {
        paint_step[i]*=-1;
      }
      else if(task5_coords_record[task5_count_paint][i]==task5_coords_record[task5_count_paint-1][i])
      {
        paint_step[i]=0;
      }
    }
  }

}
//hh();

//此为双板对话版本，不过按键板将暂时用自制简陋板替代
//按键板只发不接，上位机只接不发
//计划用于任务3、5的视频演示及三面答辩（禁用任务1模式2）
