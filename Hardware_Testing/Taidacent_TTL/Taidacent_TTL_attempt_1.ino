// Basic code for the Taidacent TTL sensor

const int sensor_input = A0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Starting up...");
  pinMode(sensor_input, INPUT);

}

void loop() {
  /* 
  Lower numbers = fewer VOCs
  According to the little sheet of paper they gave in the package, analog outputs of
  0.1V - 0.3V is a clean environment, with unclean reaching up to 4V MAX.
  */

  float reading = analogRead(sensor_input)*5.0/1024;
  Serial.println(String(reading) + "V");
  Serial.println("Raw input: " + String(analogRead(sensor_input)));
  delay(1000);
}
