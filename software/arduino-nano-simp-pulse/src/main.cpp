// ============================================================================
//              I2C Quadrature Encoder for Arduino Nano
// ============================================================================

// This program is an I2C device, that counts pulses from 2 quadrature encoders.

#include "Encoder.h"
#include <Wire.h>

// --- Quadrature Encoder Constants -------------------------------------------
// Interrupt capable pins on Arduino Nano: D2, D3
// Each encoder gets one interrupt pin.
byte const ENC_1_PIN_1 = 2;
byte const ENC_1_PIN_2 = 4;
byte const ENC_2_PIN_1 = 3;
byte const ENC_2_PIN_2 = 5;
// Pins for encoder direction jumpers.
byte const ENC_1_DIRECTION_PIN = 6;
byte const ENC_2_DIRECTION_PIN = 7;

// --- I2C Constants ----------------------------------------------------------
// I2C Pins on Arduino Nano:  A4 (SDA) and A5 (SCL)
// Base address
byte const I2C_ADDR_BASE = 0x28;
// Lowest address bits can be chosen with jumpers
byte const I2C_ADDR_PIN_1 = 11;
byte const I2C_ADDR_PIN_2 = 12;

// ---I2C Registers -------------------
// No register is selected
byte const REG_NONE = 0;
// Identifies the device, readable, 1 byte
byte const REG_WHOAMI = 0x01;
// Reset all counters to a certain value, writable, 1 long
byte const REG_RESET = 0x0C;
// The counters, readable, 2 long
byte const REG_COUNT = 0x10;

// --- Constants for low frequency activity LED -------------------------------
// Time between checks for activity, in microseconds. Also blink frequency / 2.
unsigned long const BLINK_US = 250000L;
// Estimated average duration of the main loop in microseconds.
unsigned long const LOOP_US = 20;
unsigned long const LOOP_COUNTER_START = BLINK_US / LOOP_US;

// --- Global Variables -------------------------------------------------------
// Selector for the internal registers.
byte cmdReg = REG_NONE;
// The reader objects for the encoders.
Encoder enc_1(ENC_1_PIN_1, ENC_1_PIN_2);
Encoder enc_2(ENC_2_PIN_1, ENC_2_PIN_2);
// Temporary counters for sending on I2C-Bus.
// `Encoder::read` can't be called inside `requestEvent`.
long temp_counter_1 = 0;
long temp_counter_2 = 0;
// Low frequency activity LED: state and counters.
bool led_state = LOW;
unsigned long loop_counter = LOOP_COUNTER_START;
long old_counter_1 = 0;
long old_counter_2 = 0;


// Function that executes whenever data is received from master.
// This function is registered as an event, see `setup()`.
void receiveEvent(int _) {
  while (Wire.available() > 0) {
    switch (cmdReg) {
      // If the register is not set, the current byte is
      // interpreted as the register.
      case REG_NONE:
        cmdReg = Wire.read();
        //Serial.print("Register: ");
        //Serial.println(cmdReg, HEX);
 
        // Also read the counters because we can't read them in the `requestEvent` function.
        temp_counter_1 = enc_1.read();
        temp_counter_2 = enc_2.read();
        break;

      // Command: Reset the counters to a specified value.
      case REG_RESET:
        byte buf[4]; // long is 4 bytes
        buf[3] = Wire.read();
        buf[2] = Wire.read();
        buf[1] = Wire.read();
        buf[0] = Wire.read();
        // TODO: No copying, make `buf` only a pointer to `newPosition`.
        long newPosition = *(long *)buf;
        enc_1.write(newPosition);
        enc_2.write(newPosition);
        //Serial.print("Reset. Receive new value: ");
        //Serial.println(newPosition, DEC);
 
        // The command is finished, reset the register state
        cmdReg = REG_NONE;
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
        cmdReg = REG_NONE;
        break;
    }
  }
}


// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  switch (cmdReg) {
    // Command: send the identification code
    case REG_WHOAMI:
      //Serial.println("Who am I. Send: 0xOd");
      Wire.write(0x0d);
      // The command is finished, reset the register state
      cmdReg = REG_NONE;
      break;

    // Command: Send the counter values
    case REG_COUNT:
      //Serial.print("Send counter values. 1: ");
      //Serial.println(enc_1.read(), DEC);
      byte buf[4]; // long is 4 bytes
      //*(long *)buf = enc_1.read();
      // TODO: No copying, make `buf` only a pointer to `newPosition`.
      *(long *)buf = temp_counter_1;
      Wire.write(buf[3]);
      Wire.write(buf[2]);
      Wire.write(buf[1]);
      Wire.write(buf[0]);
 
      //Serial.print(", 2: ");
      //Serial.println(enc_2.read(), DEC);
      //*(long *)buf = enc_2.read();
      // TODO: No copying, make `buf` only a pointer to `newPosition`.
      *(long *)buf = temp_counter_2;
      Wire.write(buf[3]);
      Wire.write(buf[2]);
      Wire.write(buf[1]);
      Wire.write(buf[0]);
 
      // The command is finished, reset the register state
      cmdReg = REG_NONE;
      break;

    // Error
    default:
      //Serial.println("Error! Send: 0");
      Wire.write(0x00);
      // Reset the register
      cmdReg = REG_NONE;
      break;
  }
}


// --- Startup -----------------------------------------------------------------
// Function that is called once at startup.
void setup() {
    // Init I2C -----------------------
    // Compute I2C address, respecting address jumpers.
    // Address jumpers must be connected to ground.
    pinMode(I2C_ADDR_PIN_1, INPUT_PULLUP);
    pinMode(I2C_ADDR_PIN_2, INPUT_PULLUP);
    byte i2c_address = I2C_ADDR_BASE;
    if (digitalRead(I2C_ADDR_PIN_1) == LOW) {
        i2c_address += 1;
    }
    if (digitalRead(I2C_ADDR_PIN_2) == LOW) {
        i2c_address += 2;
    }
    // Init I2C subsystem
    Wire.begin(i2c_address);         // join i2c bus as slave
    Wire.onReceive(receiveEvent); // register event
    Wire.onRequest(requestEvent); // register event
    // Switch the pullup resistors off for the I2C pins.
    digitalWrite(SDA, LOW);
    digitalWrite(SCL, LOW);

    // Init activity LED --------------
    pinMode(LED_BUILTIN, OUTPUT);

    // Init encoder library --------------
    // Direction jumpers must be connected to ground.
    pinMode(ENC_1_DIRECTION_PIN, INPUT_PULLUP);
    pinMode(ENC_2_DIRECTION_PIN, INPUT_PULLUP);
    if (digitalRead(ENC_1_DIRECTION_PIN) == LOW) {
        enc_1 = Encoder(ENC_1_PIN_2, ENC_1_PIN_1);
    }
    else {
        enc_1 = Encoder(ENC_1_PIN_1, ENC_1_PIN_2);
    }
    if (digitalRead(ENC_2_DIRECTION_PIN) == LOW)
    {
        enc_2 = Encoder(ENC_2_PIN_2, ENC_2_PIN_2);
    }
    else {
        enc_2 = Encoder(ENC_2_PIN_1, ENC_2_PIN_2);
    }

    // start serial for output --------
    //Serial.begin(9600);
    //Serial.println("I2C Test");
}


// --- Run --------------------------------------------------------------------
// Function that is called forever in a loop.
void loop() {
  // Read the encoders because they have only one interrupt pin.
  long counter_1, counter_2;
  counter_1 = enc_1.read();
  counter_2 = enc_2.read();
 
  // Decrement counter for low frequency LED.
  -- loop_counter;
  if (loop_counter == 0) {
    loop_counter = LOOP_COUNTER_START;
 
    // Blink the LED, if one of the counters has changed.
    if (  (counter_1 != old_counter_1)
       or (counter_2 != old_counter_2)
       ) {
        old_counter_1 = counter_1;
        old_counter_2 = counter_2;
        led_state = !led_state;
        digitalWrite(LED_BUILTIN, led_state);
        //Serial.print("Blink led, counters 1: ");
        //Serial.print(counter_1, DEC);
        //Serial.print(" 2: ");
        //Serial.println(counter_2, DEC);
    }
  }
}
