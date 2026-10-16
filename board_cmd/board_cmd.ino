unsigned long last_time;

void setup()
{
  Serial.begin(9600);
  pinMode(9,INPUT_PULLUP);//按键1
  pinMode(10,INPUT_PULLUP);//按键2
  pinMode(11,INPUT_PULLUP);//按键3
  pinMode(12,INPUT_PULLUP);//按键4
  pinMode(13,INPUT_PULLUP);//按键5


}

void loop() 
{
  //Serial.print("p");
  if(millis()-last_time>=200)
  {
    if(digitalRead(9)==0)
    {
      Serial.print("a");
    }
    else if(digitalRead(10)==0)
    {
      Serial.print("b");
    }
    else if(digitalRead(11)==0)
    {
      Serial.print("c");
    }
    else if(digitalRead(12)==0)
    {
      Serial.print("d");
    }
    else if(digitalRead(13)==0)
    {
      Serial.print("e");
    }
    last_time=millis();
  }
}
