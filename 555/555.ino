
char a;
int b,c,d;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

}

//输入格式x~,y~,z~
void loop() {
  // put your main code here, to run repeatedly:
if(Serial.available()!=0)
{
  a=Serial.read();
  if(a=='x')
  b=Serial.parseInt();
  
  a=Serial.read();
    if(a==',')
  {  a=Serial.read();
    if(a=='y')
  c=Serial.parseInt();}

a=Serial.read();
    if(a==',')
  {  a=Serial.read();
    if(a=='z')
  d=Serial.parseInt();}


 Serial.read();

  Serial.print(b);
  Serial.print(" ");
  Serial.print(c);
  Serial.print(" ");
  Serial.println(d);




  
  
}
delay(1000);
}
