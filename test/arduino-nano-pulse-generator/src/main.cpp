#include <Arduino.h>

// Output pins for quadrature signal.
int const OUT_1 = 2;
int const OUT_2 = 3;
int const OUT_3 = 4;
int const OUT_4 = 5;

// Frequency selection pins
int const FREQ_SEL_1 = 10;
int const FREQ_SEL_2 = 11;

// The LED blinks before signals are generated.
int const START_DELAY = 250;
int const START_N_BLINK = 4;
// LED blinks after signals are generated.
int const END_DELAY = 100;

// Number of cycles
unsigned int const WORK_N_CYCLES = 5000;
// Frequency of Quadrature signal and number of cycles.
double const BASE_FREQUENCY = 100; // [Hz]
// Delay of the algorithm for each cycle of the loop.
double const ALGO_DELAY = 38; // [1e-6 s]

void setup() {
  // Set the mode for the  pins. -------
  // Pulses to be counted
  pinMode(OUT_1, OUTPUT);
  pinMode(OUT_2, OUTPUT);
  pinMode(OUT_3, OUTPUT);
  pinMode(OUT_4, OUTPUT);
  digitalWrite(OUT_1, HIGH);
  digitalWrite(OUT_2, HIGH);
  digitalWrite(OUT_3, HIGH);
  digitalWrite(OUT_4, HIGH);

  // Frequency selection pins
  pinMode(FREQ_SEL_1, INPUT_PULLUP);
  pinMode(FREQ_SEL_2, INPUT_PULLUP);

  // Led that shows current status
  pinMode(LED_BUILTIN, OUTPUT);

  unsigned int i;

  // Blink a few times before generating signals.
  digitalWrite(LED_BUILTIN, LOW);
  for (i = 0; i < START_N_BLINK * 2; i++)
  {
    bool led_status = !digitalRead(LED_BUILTIN);
    digitalWrite(LED_BUILTIN, led_status);
    delay(START_DELAY);
  }

  // Compute the frequency multiplyer, that the user can set with the frequency
  // select pins.
  double freq_multiplier = 1;
  if (digitalRead(FREQ_SEL_1) == LOW) {
    freq_multiplier *= 5;
  }
  if (digitalRead(FREQ_SEL_2) == LOW) {
    freq_multiplier *= 10;
  }

  // Compute the delay for the loop
  double const work_frequency = BASE_FREQUENCY * freq_multiplier;
  double const cycle_time = 1 / work_frequency * 1e6; // [1e-6 s]
  unsigned int const work_delay = round((cycle_time - ALGO_DELAY) / 4); // [1e-6 s]

  // LED should be on while signals are generated.
  digitalWrite(LED_BUILTIN, HIGH);

  // Generate the quadrature signals.
  for (i = 0; i < WORK_N_CYCLES; i++)
  {
    digitalWrite(OUT_1, HIGH);
    digitalWrite(OUT_3, HIGH);
    delayMicroseconds(work_delay);
    digitalWrite(OUT_2, HIGH);
    digitalWrite(OUT_4, HIGH);
    delayMicroseconds(work_delay);
    digitalWrite(OUT_1, LOW);
    digitalWrite(OUT_3, LOW);
    delayMicroseconds(work_delay);
    digitalWrite(OUT_2, LOW);
    digitalWrite(OUT_4, LOW);
    delayMicroseconds(work_delay);
  }

}

void loop() {
  // Blink the LED indefinitely at the end
  digitalWrite(LED_BUILTIN, LOW);
  while (true)
  {
    bool led_status = !digitalRead(LED_BUILTIN);
    digitalWrite(LED_BUILTIN, led_status);
    delay(END_DELAY);
  }
}