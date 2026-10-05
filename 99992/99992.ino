//此代码仅针对肘关节向上的情形
int k1=1,b1=98,k0=-1,b0=179;
#define l1 10
#define l2 10
double p[3]={0,0,0};//位置坐标xyz
double a[3]={0,0,0,};//水平角αβ     γ
double aa[3]={0,0,0};//舵机角

void setup() {
Serial.begin(9600);

}


//输入目标点坐标x~y~z~;输出关节角α~β~γ~（角度值）.
//该版本下请勿输入x0y0z~.
void loop() {

if(Serial.available()!=0)
{
  if(Serial.read()=='x')
  {p[0]=Serial.parseFloat();}//不能用parseInt//更不能用read！
  if(Serial.read()=='y')
  {p[1]=Serial.parseFloat();}
  if(Serial.read()=='z')
  {p[2]=Serial.parseFloat();}
  Serial.read();
  f(p,3,angle,3);
  Serial.print("α");
  Serial.print(angle[0]);
  Serial.print("β");
  Serial.print(angle[1]);
  Serial.print("γ");
  Serial.println(angle[2]);
}


}


void f(double p[],int np,double a[],int na)
{
  double i=p[0]*p[0]+p[1]*p[1];
  double ii=p[0]*p[0]+p[1]*p[1]+p[2]*p[2];
  a[0]=(atan(p[2]/sqrt(i))+acos((ii+l1*l1-l2*l2)/(2*l1*sqrt(ii))))/PI*180;
  a[1]=-1*acos((ii-l1*l1-l2*l2)/(2*l1*l2))/PI*180+a[0];//经过校对及参考，我发现此处的α、β符号会被吞掉，当肘关节向下时：α返回值差个负号，β返回值正确；但是当肘关节向上时：α返回值正确，β返回值差个负号。此版本仅供计算肘关节向上的情形，故补一个负号。
  a[2]=asin(p[1]/sqrt(i))/PI*180;                
}                                
void g(double a[],int na,double aa[],int naa) 
{
  aa[0]=a[0]*k0+b0;aa[1]=a[1]*k1+b1;
  aa[2]=a[2]+93;
}                                                                                                       
