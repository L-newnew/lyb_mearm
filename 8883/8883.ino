int v=10;

void setup() {
Serial.begin(9600);
}

void loop() {
if(Serial.available(!=0))
{
  if(Serial.read()=='h'){v++;Serial.read;}
  if(Serial.read()=='l'){v--;Serial.read;}
}

}
