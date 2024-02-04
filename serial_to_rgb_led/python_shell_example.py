"""
An example you can run from the command line, or
directly as a script.

Author: Michele De Stefano
"""

import serial
import struct

# Try obtaining magenta
R, G, B = 255, 0, 191

# IMPORTANT! GIVE THE FOLLOWING INSTRUCTION BEFORE
# OPENING THE SERIAL MONITOR, OTHERWISE THE SERIAL
# MONITOR FORBIDS THE CONNECTION.
arduino = serial.Serial("/dev/ttyACM0", 9600)

# AT THIS POINT, YOU CAN ALSO OPEN THE SERIAL MONITOR

data = struct.pack("BBB", R, G, B)

arduino.write(data)

arduino.close()

