import serial
from time import sleep
from matplotlib import pyplot as plt
import numpy as np


dev = serial.Serial("/dev/ttyUSB0", 115200)
for i in range(3):
    dev.write(b'AT+BACK\r')
    dev.read(3)
bmp = []
sts = []
for i in range(300):
    dev.write(b'AT+BMP\r')
    sleep(0.1)
    dev.read(3)
    dev.write(b'AT+GETTEMP\r')
    sleep(0.1)
    value = dev.read(8)
    bmp.append(int(value[3:])/1000)
    print("%.2f" %  (int(value[3:])/1000))
    dev.write(b'AT+BACK\r')
    sleep(0.1)
    dev.read(3)
    dev.write(b'AT+STS\r')
    sleep(0.1)
    dev.read(3)
    dev.write(b'AT+GETDATA\r')
    sleep(0.1)
    value = dev.read(8)
    sts.append(int(value[3:])/1000 - 8)
    print("%.2f" %  (int(value[3:])/1000))
    dev.write(b'AT+BACK\r')
    sleep(0.1)
    dev.read(3)
    sleep(0.4)

x = np.arange(len(bmp))
plt.plot(x, bmp, 'k')
plt.plot(x, sts, 'g')
plt.xlabel("Time [s]")
plt.ylabel("Temperature [deg Celcius]")
plt.legend(["BMP", "STS"])
plt.show()
dev.close()
