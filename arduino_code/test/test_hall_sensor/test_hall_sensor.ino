int hallSensor = 2;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // make the Hall sensor's pin an input:
  pinMode(hallSensor, INPUT);
}

void loop() {
  // read the input pin:
  int sensorState = digitalRead(hallSensor);
  // change led to reflect the state of the Hall sensor
  digitalWrite(LED_BUILTIN, sensorState);
  delay(10);
}
