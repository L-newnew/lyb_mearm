//此代码仅针对肘关节向上的情形
int k2=1,b2=93,k1=1,b1=98,k0=-1,b0=179;
#define l1 8
#define l2 8
#define r 1.45
double p[3]={0,0,0};//位置坐标
double a[3]={0,0,0};//绝对角
double aa[3]={0,0,0};//舵机角

void setup() {
Serial.begin(9600);

}


//输入目标点坐标;输出舵机角（角度值）.
void loop() 
{

if(Serial.available()!=0)
{
  if(Serial.read()=='x')
  {p[0]=Serial.parseFloat();}//不能用parseInt//更不能用read！
  if(Serial.read()=='y')
  {p[1]=Serial.parseFloat();}
  if(Serial.read()=='z')
  {p[2]=Serial.parseFloat();}
  Serial.read();
  f(p,3,a,3);
  g(a,3,aa,3);

  Serial.print("α");
  Serial.print(aa[0]);
  Serial.print("β");
  Serial.print(aa[1]);
  Serial.print("γ");
  Serial.println(aa[2]);
}


}


void f(double p[],int np,double a[],int na)//由坐标算绝对角（角度制）
{
  double i=p[0]*p[0]+p[1]*p[1];
  double d=sqrt(p[0]*p[0]+p[1]*p[1])-r;
  a[0]=(atan(p[2]/d)+acos((p[2]*p[2]+d*d+l1*l1-l2*l2)/(2*l1*sqrt(p[2]*p[2]+d*d))))/PI*180;
  a[1]=-1*acos((p[2]*p[2]+d*d-l1*l1-l2*l2)/(2*l1*l2))/PI*180+a[0];
  a[2]=asin(p[1]/sqrt(i))/PI*180;                
}                                
void g(double a[],int na,double aa[],int naa) //由绝对角算舵机角
{
  aa[0]=a[0]*k0+b0;
  aa[1]=a[1]*k1+b1;
  aa[2]=a[2]*k2+b2;
}                                                                                                       
