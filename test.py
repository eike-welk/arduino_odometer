# Test the Arduino I2C device

import struct
import time
import Adafruit_PureIO.smbus

address = 0x28
reg_who = 0x01
reg_reset = 0x0C
reg_counts = 0x10

i2c = Adafruit_PureIO.smbus.SMBus(1)

def read_reg_who():
    """Read the who-am-I register."""
    #ans_who = i2c.read_byte_data(address, reg_who)
    buf = i2c.read_i2c_block_data(address, reg_who, 1)
    ans_who, = struct.unpack(">b", buf)
    print("Who am I: " + hex(ans_who))

def read_reg_counters():
    """Read the counter registers."""
    buf = i2c.read_i2c_block_data(address, reg_counts, 8)
    counter_1, counter_2 = struct.unpack(">ii", buf)
    print('Counters 1:', counter_1, ', 2:', counter_2)

def write_reg_reset(val):
    """Write the reset register."""
    buf = struct.pack(">i", val)
    i2c.write_i2c_block_data(address, reg_reset, buf)

read_reg_who()
time.sleep(0.9)

read_reg_counters()
time.sleep(0.05)
read_reg_counters()

write_reg_reset(5)
time.sleep(0.1)

#read_reg_counters()
#time.sleep(0.5)
#read_reg_counters()

#write_reg_reset(-10)
#time.sleep(0.1)
#write_reg_reset(1234567890)

