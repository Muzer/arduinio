# A (very) rough example program to interact with the arduino's firmware for
# testing purposes.
import serial
import time

ser = serial.Serial('/dev/ttyACM0', 9600)
time.sleep(5) # wait for arduino to reset
while ser.inWaiting() != 0: # flush all the buffers
    print ser.read()
ser.write("\x01\x04")
if ord(ser.read()) != 1:
    print "Incorrect length"
    exit()
if ord(ser.read()) != 0:
    print "Incorrect input"
    exit()
print "successfully initialised"
while True:
    cmds = raw_input("Enter command number ")
    cmdn = int(cmds)
    if cmdn == 0:
        outs = raw_input("Enter output ")
        outn = int(outs)
        ser.write(bytearray(((2, cmdn, outn))))
    else:
        ser.write(bytearray(((1, cmdn))))
        for i in range(ord(ser.read())):
            print ord(ser.read())
