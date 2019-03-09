

#include <Wire.h>

byte const I2C_ADDR = 0x28;

// --- Register Constants -----------------------------------------------------
// No register is selected
byte const REG_NONE = 0;
// Identifies the device, readable, 1 byte
byte const REG_WHOAMI = 0x01;
// Reset all counters to a certain value, writable, 1 long
byte const REG_RESET = 0x0C;
// The counters, readable, 2 long
byte const REG_COUNT = 0x10;

// --- Global Variables -------------------------------------------------------
// Selector for the internal registers.
byte cmdReg = REG_NONE;
// The counters
long counter_1 = 0;
long counter_2 = 0;

// --- Startup -----------------------------------------------------------------
// Function that is called once at startup.
void setup() {
  Wire.begin(I2C_ADDR);         // join i2c bus as slave
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent); // register event
  // Switch the pullup resistors off for the I2C pins
  digitalWrite(SDA, LOW); 
  digitalWrite(SCL, LOW);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);           // start serial for output
  Serial.println("I2C Test");
}

// --- Run --------------------------------------------------------------------
// Function that is called forever in a loop.
void loop() {
  // Increment counter and blink the led.
  counter_1 += 1;
  counter_2 += 2;
  digitalWrite(LED_BUILTIN, counter_1 & 0x01);

  delay(1000);
}

// Function that executes whenever data is received from master.
// This function is registered as an event, see `setup()`.
void receiveEvent(int _) {
  while (Wire.available() > 0) {
    switch (cmdReg) {
      // If the register is not set, the current byte is
      // interpreted as the register.
      case REG_NONE:
        cmdReg = Wire.read();
        Serial.print("Register: ");
        Serial.println(cmdReg, HEX);
        break;

      // Command: Reset the counters to a specified value.
      case REG_RESET:
        byte buf[4]; // long is 4 bytes
        buf[3] = Wire.read();
        buf[2] = Wire.read();
        buf[1] = Wire.read();
        buf[0] = Wire.read();
        counter_1 = counter_2 = *(long *)buf;
        Serial.print("Reset. Receive new value: ");
        Serial.println(counter_1, DEC);
 
        // The command is finished, reset the register state
        cmdReg = REG_NONE;
        break;

      // Error: Read all bytes in this transaction
      default:
        Serial.print("Error! Receive: ");
        while (0 < Wire.available()) {
          byte errByte = Wire.read();
          Serial.print(errByte, HEX);
          Serial.print(", ");
        }
        Serial.println("");
 
        // Reset the register state, wait for new regiser/command
        cmdReg = REG_NONE;
        break;
    }
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  switch (cmdReg) {
    // Command: send the identifiacation code
    case REG_WHOAMI:
      Serial.println("Who am I. Send: 0xOd");
      Wire.write(0x0d);
      // The command is finished, reset the register state
      cmdReg = REG_NONE;
      break;

    // Command: Send the counter values
    case REG_COUNT:
      Serial.print("Send counter values. 1: ");
      Serial.print(counter_1, DEC);
      byte buf[4]; // long is 4 bytes
      *(long *)buf = counter_1;
      Wire.write(buf[3]);
      Wire.write(buf[2]);
      Wire.write(buf[1]);
      Wire.write(buf[0]);
 
      Serial.print(", 2: ");
      Serial.println(counter_2, DEC);
      *(long *)buf = counter_2;
      Wire.write(buf[3]);
      Wire.write(buf[2]);
      Wire.write(buf[1]);
      Wire.write(buf[0]);
 
      // The command is finished, reset the register state
      cmdReg = REG_NONE;
      break;

    // Error
    default:
      Serial.println("Error! Send: 0");
      Wire.write(0x00);
      // Reset the register
      cmdReg = REG_NONE;
      break;
  }
}

