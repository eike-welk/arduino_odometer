// ============================================================================
//              I2C Quadrature Encoder for Arduino Nano
// ============================================================================

// This program is an I2C device, that counts pulses from four (Hall) sensors.
//
// This program uses polling, to read four pins, which is fast enough and
// simple.
//
// Polling does also not interfere with I2C, which uses interrupts internally.
// However I2C interferes with polling, because it uses considerable amounts of
// time and makes the program potentially miss counts.
//
// The 16 MHz Arduino Nano was tested with 5kHz pulses on each pin and worked 
// well. A 8 MHz version might start to miss pulses at 5 kHz, during I2C
// communication.
//
// The Arduino Nano has only two Pins with fast "Pin Interrupts": D2, D3.
// Additionally the Nano has "Pin Change Interrupts" which work on all pins,
// but are more complicated and slower.

#include "Arduino.h"
#include <Wire.h>


// Use the RL-Pins for debug and test output
#define DEBUG_RL_PINS false

// --- Constants ---------------------------------------------------------------
// --- Pulse Input Constants ------------------------------
// Pulse counter pins: There are two plugs with two inputs each.
// (Pins with "Pin Interrupts": D2, D3.)
byte const PLUG_1_PIN_1 = 2;
byte const PLUG_1_PIN_2 = 4;
byte const PLUG_2_PIN_1 = 3;
byte const PLUG_2_PIN_2 = 5;
// RL-Pins: Pins for jumpers to swap left and right side on each plug.
byte const PLUG_1_RL_PIN = 11;
byte const PLUG_2_RL_PIN = 10;

// --- I2C Constants --------------------------------------
// I2C Pins on Arduino Nano:  A4 (SDA) and A5 (SCL)
// Base address
byte const I2C_ADDR_BASE = 0x28;
// Lowest address bits can be chosen with jumpers
byte const I2C_ADDR_PIN_1 = 12;
byte const I2C_ADDR_PIN_2 = 13;

// --- Commands that the odometer understands -------------
// No command is executing
byte const CMD_NONE = 0;
// Identifies the device, sends 6 bytes over I2C.
byte const CMD_WHOAMI = 0x01;
// Reset all counters to a certain value, reads 1 int32_t.
byte const CMD_RESET = 0x0C;
// Send the counter values, sends 4 int32_t over I2C.
byte const CMD_GET_COUNT = 0x10;

// Response string for CMD_WHOAMI
byte const WHOAMI_RESP[] = {"odsp01"};

// --- Low frequency activity LED -------------------------
// Pause between invocations of the blink algorithm. 
// One blink cycle is two pauses.
unsigned long const ACTIVITY_BLINK_MILLIS = 500;

// --- Global Variables --------------------------------------------------------
// I2C ---------------------------------------------------
// Command that is currently executed.
byte cmdState = CMD_NONE;

// Counting -----------------------------------------------
// Current pin state
bool pin_state_1_1 = false;
bool pin_state_1_2 = false;
bool pin_state_2_1 = false;
bool pin_state_2_2 = false;
// The main counters of the odometer.
int32_t counter_1_1 = 0;
int32_t counter_1_2 = 0;
int32_t counter_2_1 = 0;
int32_t counter_2_2 = 0;
// Buffers to convert the counters to network order, for I2C.
int const COUNTER_BUFFER_LENGTH = 4 * sizeof(int32_t);
byte counter_buffer_1[COUNTER_BUFFER_LENGTH] = {0};
byte counter_buffer_2[COUNTER_BUFFER_LENGTH] = {0};
// Indexes into the buffer for each counter.
// They are not constants because they can be swapped during initialization.
int buf_index_1_1 = 0 * sizeof(int32_t);
int buf_index_1_2 = 1 * sizeof(int32_t);
int buf_index_2_1 = 2 * sizeof(int32_t);
int buf_index_2_2 = 3 * sizeof(int32_t);
// Pointer to buffer that is currently filled.
byte * new_buffer = &counter_buffer_1[0];
// Pointer to buffer that can be written over I2C.
byte * active_buffer = &counter_buffer_2[0];

// Low frequency activity LED -----------------------------
// LED state
bool led_state = LOW;
//  Value of `millis()` at last blink.
unsigned long last_activity_blink = 0;
//  Old values of the counters.
int32_t old_counter_1_1 = 0; // Plug 1
int32_t old_counter_1_2 = 0;
int32_t old_counter_2_1 = 0; // Plug 2
int32_t old_counter_2_2 = 0;


// I2C Functions ---------------------------------------------------------------
// Function to convert a int32_t into bytes in network order.
void convert_to_network(int32_t const num, byte * buf) {
  buf[3] = num & 0xFF;
  buf[2] = (num >> 8) & 0xFF;
  buf[1] = (num >> 16) & 0xFF;
  buf[0] = (num >> 24) & 0xFF;
}
/* // Alternative algorithm int32_t -> int32_t 
// #define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
//                    ((x)<< 8 & 0x00FF0000UL) | \
//                    ((x)>> 8 & 0x0000FF00UL) | \
//                    ((x)>>24 & 0x000000FFUL) )
*/


// Function that executes whenever data is received from master.
// This function is registered as an event, see `setup()`.
void receiveEvent(int _) {
  #if DEBUG_RL_PINS
    digitalWrite(PLUG_2_RL_PIN, true);
  #endif

  while (Wire.available() > 0) {
    switch (cmdState) {
      // If the register is not set, the current byte is
      // interpreted as the register.
      case CMD_NONE:
        cmdState = Wire.read();
        //Serial.print("Register: ");
        //Serial.println(cmdState, HEX);
        break;

      // Command: Reset the counters to a specified value.
      case CMD_RESET:
      {
        byte b1, b2, b3, b4;
        int32_t new_counter;

        b1 = Wire.read();
        b2 = Wire.read();
        b3 = Wire.read();
        b4 = Wire.read();

        // Convert the data from network order to host order.
        new_counter = b1;
        new_counter = (new_counter << 8) | b2;
        new_counter = (new_counter << 8) | b3;
        new_counter = (new_counter << 8) | b4;

        counter_1_1 = new_counter;
        counter_1_2 = new_counter;
        counter_2_1 = new_counter;
        counter_2_2 = new_counter;

        //Serial.print("Reset. Receive new value: ");
        //Serial.println(new_counter, DEC);

        // If the master sent more bytes, this is an error.
        if (Wire.available())
          goto i2c_receive_error;

        // The command is finished, reset the register state
        cmdState = CMD_NONE;
        break;
      }

      // Error: Read all bytes in this transaction
      i2c_receive_error:
      default:
      {
        //Serial.print("Error! Receive: ");
        while (0 < Wire.available()) {
          Wire.read();
          // byte errByte = Wire.read();
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

  #if DEBUG_RL_PINS
    digitalWrite(PLUG_2_RL_PIN, false);
  #endif
}


// Function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  #if DEBUG_RL_PINS
    digitalWrite(PLUG_2_RL_PIN, true);
  #endif

  switch (cmdState) {
    // Command: send the identification code
    case CMD_WHOAMI:
      //Serial.println("Who am I.");
      Wire.write(WHOAMI_RESP, sizeof(WHOAMI_RESP));
      // The command is finished, reset the register state
      cmdState = CMD_NONE;
      break;

    // Command: Send the counter values
    case CMD_GET_COUNT:
      //Serial.print("Send counter values.");
      Wire.write(active_buffer, COUNTER_BUFFER_LENGTH);
 
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

  #if DEBUG_RL_PINS
    digitalWrite(PLUG_2_RL_PIN, false);
  #endif
}


// --- Startup -----------------------------------------------------------------
// Function that is called once at startup.
void setup()
{
  // Init I2C ------------------------
  // Compute I2C address, respecting the address jumpers.
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

  // Configure counting ----------------
  // Pulse input pins
  pinMode(PLUG_1_PIN_1, INPUT_PULLUP);
  pinMode(PLUG_1_PIN_2, INPUT_PULLUP);
  pinMode(PLUG_2_PIN_1, INPUT_PULLUP);
  pinMode(PLUG_2_PIN_2, INPUT_PULLUP);
  // Right - Left exchange jumpers 
  // The RL jumpers connect the pins to ground.
  pinMode(PLUG_1_RL_PIN, INPUT_PULLUP);
  pinMode(PLUG_2_RL_PIN, INPUT_PULLUP);
  // Swap the positions of the left and right counters in the I2C buffer.
  int temp_index;
  if (digitalRead(PLUG_1_RL_PIN) == LOW) { 
    temp_index = buf_index_1_1;
    buf_index_1_1 = buf_index_1_2;
    buf_index_1_2 = temp_index;
  }
  if (digitalRead(PLUG_2_RL_PIN) == LOW) { 
    temp_index = buf_index_2_1;
    buf_index_2_1 = buf_index_2_2;
    buf_index_2_2 = temp_index;
  }

  // Init activity LED -----------------
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, led_state);

  // start serial for output -----------
  //Serial.begin(9600);
  //Serial.println("Setup complete.");

  // Use the RL-Pins for debug output ---
  #if DEBUG_RL_PINS
    pinMode(PLUG_1_RL_PIN, OUTPUT);
    pinMode(PLUG_2_RL_PIN, OUTPUT);
    digitalWrite(PLUG_1_RL_PIN, false);
    digitalWrite(PLUG_2_RL_PIN, false);
  #endif
}


// --- Run --------------------------------------------------------------------
// Function that is called forever in a loop.
void loop()
{
  #if DEBUG_RL_PINS
    digitalWrite(PLUG_1_RL_PIN, true);
  #endif

  // Compute the main counters -----------------------------
  bool curr_state;
  // Read each pin. If its current state is different than its previous state,
  // increment the counter.
  curr_state = digitalRead(PLUG_1_PIN_1);
  if (pin_state_1_1 != curr_state) {
    pin_state_1_1 = curr_state;
    ++counter_1_1;
  }
  curr_state = digitalRead(PLUG_1_PIN_2);
  if (pin_state_1_2 != curr_state) {
    pin_state_1_2 = curr_state;
    ++counter_1_2;
  }
  curr_state = digitalRead(PLUG_2_PIN_1);
  if (pin_state_2_1 != curr_state) {
    pin_state_2_1 = curr_state;
    ++counter_2_1;
  }
  curr_state = digitalRead(PLUG_2_PIN_2);
  if (pin_state_2_2 != curr_state) {
    pin_state_2_2 = curr_state;
    ++counter_2_2;
  }

  // Fill buffer that can be sent over I2c ---------------
  convert_to_network(counter_1_1, &new_buffer[buf_index_1_1]);
  convert_to_network(counter_1_2, &new_buffer[buf_index_1_2]);
  convert_to_network(counter_2_1, &new_buffer[buf_index_2_1]);
  convert_to_network(counter_2_2, &new_buffer[buf_index_2_2]);
  // swap the buffers
  byte * temp_ptr = new_buffer;
  new_buffer = active_buffer;
  noInterrupts();
  active_buffer = temp_ptr;
  interrupts();

  // Low frequency activity LED ----------------------------
  unsigned long current_millis = millis();
  // Blink the LED, if enough time has elapsed ...
  if (current_millis - last_activity_blink > ACTIVITY_BLINK_MILLIS)
  {
    last_activity_blink = current_millis;

    // (Blink the LED) and if one of the counters has changed.
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

  #if DEBUG_RL_PINS
    digitalWrite(PLUG_1_RL_PIN, false);
    // delay(1);
  #endif
}
