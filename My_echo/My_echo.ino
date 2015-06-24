/*
 * Reflect serial input back.
 */
 byte c;
void setup()
{
  Serial.begin(9600);
}

void loop()
{
  while (Serial.available() > 0) {
    c=Serial.read();
    Serial.write(c);
    Serial.print(c);
  }
}
