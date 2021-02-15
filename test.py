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
    buf = i2c.read_i2c_block_data(address, reg_who, 7)
    print(buf)
    ans_who, = struct.unpack("!6sx", buf)
    ans_who_str = ans_who.decode('utf-8')
    print("Who am I: ", ans_who_str)

def read_reg_counters():
    """Read the counter registers."""
    buf = i2c.read_i2c_block_data(address, reg_counts, 16)
    counter_1, counter_2, counter_3, counter_4 = struct.unpack('!4i', buf)
    print('Counters 1:', counter_1, ', 2:', counter_2, ', 3:', counter_3, ', 4:', counter_4)

def write_reg_reset(val):
    """Write the reset register."""
    buf = struct.pack("!i", val)
    i2c.write_i2c_block_data(address, reg_reset, buf)

read_reg_who()
#time.sleep(0.9)
write_reg_reset(0)
#time.sleep(0.1)

while True:
    read_reg_counters()
    time.sleep(0.05)
    #time.sleep(0.01)

time.sleep(0.1)

#read_reg_counters()
#time.sleep(0.5)
#read_reg_counters()

#write_reg_reset(-10)
#time.sleep(0.1)
#write_reg_reset(1234567890)

