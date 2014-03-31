const int SENSOR_PIN = A5;
boolean ballInserted = false;

void setup()
{
  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT);
}

void loop()
{
  int analogValue = analogRead(SENSOR_PIN);
  
  if (analogValue > 250 && false == ballInserted)
  {
    ballInserted = true;
    Serial.println("Ball insert begin");
  }
  else if (analogValue <= 250 && true == ballInserted)
  {
    ballInserted = false;
    Serial.println("Ball insert end");
  }
}
