//此代码仅针对肘关节向上的情形//位置坐标直接算舵机角,舵机角直接算位置坐标
int k2=1,b2=93,k1=1,b1=98,k0=-1,b0=179;
#define l1 8
#define l2 8
#define r 1.45
double p[3]={0,0,0};//位置坐标
double a[3]={0,0,0};//绝对角
double aa[3]={0,0,0};//舵机角

void setup() 
{
Serial.begin(9600);
}


//输入目标点坐标;输出舵机角（角度值）.
void loop() 
{



}



void h(double p[],int np,double a[],int na,double aa[],int naa) //位置坐标直接算舵机角 
{
  double i=p[0]*p[0]+p[1]*p[1];
  double d=sqrt(p[0]*p[0]+p[1]*p[1])-r;
  //辅助计算
  

  a[0]=(atan(p[2]/d)+acos((p[2]*p[2]+d*d+l1*l1-l2*l2)/(2*l1*sqrt(p[2]*p[2]+d*d))))/PI*180;
  a[1]=-1*acos((p[2]*p[2]+d*d-l1*l1-l2*l2)/(2*l1*l2))/PI*180+a[0];
  a[2]=asin(p[1]/sqrt(i))/PI*180;      

  aa[0]=a[0]*k0+b0;
  aa[1]=a[1]*k1+b1;
  aa[2]=a[2]*k2+b2;
}          
//h(p,3,a,3,aa,3);               


void hh(double p[],int np,double a[],int na,double aa[],int naa)//舵机角直接算位置坐标
{
  a[0]=179-aa[0];
  a[1]=aa[1]-98;
  a[2]=aa[2]-93;

  p[0]=cos(a[2])*(l1*cos(a[0])+l2*cos(a[1])+r);
  p[1]=sin(a[2])*(l1*cos(a[0])+l2*cos(a[1])+r);
  p[2]=l1*sin(a[0])+l2*sin(a[1]);
}
//hh(p,3,a,3,aa,3);
