// ============================================================================
//              I2C Quadrature Encoder for Arduino Nano
// ============================================================================

// This program is an I2C device, that counts pulses from 2 quadrature encoders.

#include "PinChangeInterrupt.h"
#include <Wire.h>

// --- Quadrature Encoder Constants -------------------------------------------
// Pins with "Pin Interrupts" on Arduino Nano: D2, D3
// This program however uses "Pin Change Interrupts" which work on many pins,
// but are more complicated and slower.
byte const PLUG_1_PIN_1 = 2;
byte const PLUG_1_PIN_2 = 4;
byte const PLUG_2_PIN_1 = 3;
byte const PLUG_2_PIN_2 = 5;
// Pins for jumpers to swap left and right wheels on each plug.
byte const PLUG_1_RL_PIN = 6;
byte const PLUG_2_RL_PIN = 7;

// --- I2C Constants ----------------------------------------------------------
// I2C Pins on Arduino Nano:  A4 (SDA) and A5 (SCL)
// Base address
byte const I2C_ADDR_BASE = 0x28;
// Lowest address bits can be chosen with jumpers
byte const I2C_ADDR_PIN_1 = 11;
byte const I2C_ADDR_PIN_2 = 12;

// --- Commands that the odometer understands -------------------
// No command is executing
byte const CMD_NONE = 0;
// Identifies the device, sends 6 bytes over I2C.
byte const CMD_WHOAMI = 0x01;
// Reset all counters to a certain value, reads 1 long.
byte const CMD_RESET = 0x0C;
// Send the counter values, sends 4 long over I2C.
byte const CMD_GET_COUNT = 0x10;

// --- Constants for low frequency activity LED -------------------------------
// Time between checks for activity, in microseconds. Also blink frequency / 2.
unsigned long const BLINK_US = 250000L;
// Estimated average duration of the main loop in microseconds.
unsigned long const LOOP_US = 20;
unsigned long int LOOP_COUNTER_START = BLINK_US / LOOP_US;

// --- Global Variables -------------------------------------------------------
// Command that is currently executed.
byte cmdState = CMD_NONE;
// Interrupt numbers, computed in setup()
byte intr_num_1_1;
byte intr_num_1_2;
byte intr_num_2_1;
byte intr_num_2_2;
// Fast counters for the interrupt routines
byte volatile intr_counter_1_1 = 0; // Plug 1
byte volatile intr_counter_1_2 = 0;
byte volatile intr_counter_2_1 = 0; // Plug 2
byte volatile intr_counter_2_2 = 0;
// The main counters of the odometer
long counter_1_1 = 0; // Plug 1
long counter_1_2 = 0;
long counter_2_1 = 0; // Plug 2
long counter_2_2 = 0;
// Low frequency activity LED: state and counters.
bool led_state = LOW;
unsigned int loop_counter = LOOP_COUNTER_START;
long old_counter_1_1 = 0; // Plug 1
long old_counter_1_2 = 0;
long old_counter_2_1 = 0; // Plug 2
long old_counter_2_2 = 0;


// Function that executes whenever data is received from master.
// This function is registered as an event, see `setup()`.
void receiveEvent(int _) {
  while (Wire.available() > 0) {
    switch (cmdState) {
      // If the register is not set, the current byte is
      // interpreted as the register.
      case CMD_NONE:
        cmdState = Wire.read();
        //Serial.print("Register: ");
        //Serial.println(cmdState, HEX);
 
        // Also read the counters because we can't read them in the `requestEvent` function.
        // counter_1_1 = enc_1.read();
        // counter_1_2 = enc_2.read();
        break;

      // Command: Reset the counters to a specified value.
      case CMD_RESET:
        byte buf[4]; // long is 4 bytes
        // Read the bytes in network order
        buf[3] = Wire.read();
        buf[2] = Wire.read();
        buf[1] = Wire.read();
        buf[0] = Wire.read();
        // TODO: No copying, make `buf` only a pointer to `newPosition`.
        long newPosition = *(long *)buf;
        // enc_1.write(newPosition);
        // enc_2.write(newPosition);
        //Serial.print("Reset. Receive new value: ");
        //Serial.println(newPosition, DEC);
 
        // The command is finished, reset the register state
        cmdState = CMD_NONE;
        break;

      // Error: Read all bytes in this transaction
      default:
        //Serial.print("Error! Receive: ");
        while (0 < Wire.available()) {
          byte errByte = Wire.read();
          //Serial.print(errByte, HEX);
          //Serial.print(", ");
        }
        //Serial.println("");
 
        // Reset the register state, wait for new register/command
        cmdState = CMD_NONE;
        break;
    }
  }
}


// Function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  switch (cmdState) {
    // Command: send the identification code
    case CMD_WHOAMI:
      //Serial.println("Who am I.");
      Wire.write("odsp01", 6);
      // The command is finished, reset the register state
      cmdState = CMD_NONE;
      break;

    // Command: Send the counter values
    case CMD_GET_COUNT:
      //Serial.print("Send counter values. 1: ");
      //Serial.println(enc_1.read(), DEC);
      byte buf[4]; // long is 4 bytes
      //*(long *)buf = enc_1.read();
      // TODO: No copying, make `buf` only a pointer to `newPosition`.
      *(long *)buf = counter_1_1;
      // Write the bytes in network order
      Wire.write(buf[3]);
      Wire.write(buf[2]);
      Wire.write(buf[1]);
      Wire.write(buf[0]);
 
      //Serial.print(", 2: ");
      //Serial.println(enc_2.read(), DEC);
      //*(long *)buf = enc_2.read();
      // TODO: No copying, make `buf` only a pointer to `newPosition`.
      *(long *)buf = counter_1_2;
      // Write the bytes in network order
      Wire.write(buf[3]);
      Wire.write(buf[2]);
      Wire.write(buf[1]);
      Wire.write(buf[0]);
 
      // The command is finished, reset the register state
      cmdState = CMD_NONE;
      break;

    // Error
    default:
      //Serial.println("Error! Send: 0");
      Wire.write(0x00);
      // Reset the register
      cmdState = CMD_NONE;
      break;
  }
}

// --- Interrupt routines ------------------------------------------------------
void count_step_1_1(void) { intr_counter_1_1 ++; }
void count_step_1_2(void) { intr_counter_1_2 ++; }
void count_step_2_1(void) { intr_counter_2_1 ++; }
void count_step_2_2(void) { intr_counter_2_2 ++; }

// --- Startup -----------------------------------------------------------------
// Function that is called once at startup.
void setup()
{
  // Init I2C ------------------------
  // Compute I2C address, respecting address jumpers.
  // The Address jumpers connect the pins to ground.
  pinMode(I2C_ADDR_PIN_1, INPUT_PULLUP);
  pinMode(I2C_ADDR_PIN_2, INPUT_PULLUP);
  byte i2c_address = I2C_ADDR_BASE;
  if (digitalRead(I2C_ADDR_PIN_1) == LOW) { i2c_address += 1; }
  if (digitalRead(I2C_ADDR_PIN_2) == LOW) { i2c_address += 2; }
  // Init I2C subsystem
  Wire.begin(i2c_address);      // join i2c bus as slave
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  // Switch the pullup resistors off for the I2C pins.
  // As this is a 5V board, and RaspberryPi is 3.3 V.
  digitalWrite(SDA, LOW);
  digitalWrite(SCL, LOW);

  // TODO: Right - Left exchange jumpers ---
  // The RL jumpers connected the pins to ground.
  pinMode(PLUG_1_RL_PIN, INPUT_PULLUP);
  pinMode(PLUG_2_RL_PIN, INPUT_PULLUP);
  if (digitalRead(PLUG_1_RL_PIN) == LOW)
  {
  }
  if (digitalRead(PLUG_2_RL_PIN) == LOW)
  {
  }

  // Configure the interupt functions
  intr_num_1_1 = digitalPinToPinChangeInterrupt(PLUG_1_PIN_1);
  intr_num_1_2 = digitalPinToPinChangeInterrupt(PLUG_1_PIN_2);
  intr_num_2_1 = digitalPinToPinChangeInterrupt(PLUG_2_PIN_1);
  intr_num_2_2 = digitalPinToPinChangeInterrupt(PLUG_2_PIN_2);
  attachPinChangeInterrupt(intr_num_1_1, count_step_1_1, CHANGE);
  attachPinChangeInterrupt(intr_num_1_2, count_step_1_2, CHANGE);
  attachPinChangeInterrupt(intr_num_2_1, count_step_2_1, CHANGE);
  attachPinChangeInterrupt(intr_num_2_2, count_step_2_2, CHANGE);

  // Init activity LED ---------------
  // led_state = LOW;
  // loop_counter = LOOP_COUNTER_START;
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, led_state);

  // start serial for output --------
  //Serial.begin(9600);
  //Serial.println("Setup complete.");
}

// --- Run --------------------------------------------------------------------
// Function that is called forever in a loop.
void loop()
{
  // Add the small interupt counters to the main counters. ---
  noInterrupts();
  counter_1_1 += intr_counter_1_1; intr_counter_1_1 = 0;
  counter_1_2 += intr_counter_1_2; intr_counter_1_2 = 0;
  counter_2_1 += intr_counter_2_1; intr_counter_2_1 = 0;
  counter_2_2 += intr_counter_2_2; intr_counter_2_2 = 0;
  interrupts();

  // Low frequency activity LED --------
  // Decrement counter for LED.
  --loop_counter;
  if (loop_counter == 0)
  {
    loop_counter = LOOP_COUNTER_START;

    // Blink the LED, if one of the counters has changed.
    if (  (counter_1_1 != old_counter_1_1) 
       or (counter_1_2 != old_counter_1_2) 
       or (counter_2_1 != old_counter_2_1) 
       or (counter_2_2 != old_counter_2_2)
       )
    {
      old_counter_1_1 = counter_1_1;
      old_counter_1_2 = counter_1_2;
      old_counter_2_1 = counter_2_1;
      old_counter_2_2 = counter_2_2;
      led_state = !led_state;
      digitalWrite(LED_BUILTIN, led_state);
      // Serial.println("Blink led");
      // Serial.print("Counters on plug 1: ");
      // Serial.print(counter_1_1, DEC);
      // Serial.print(", ");
      // Serial.print(counter_1_2, DEC);
      // Serial.println();
    }
  }
}
