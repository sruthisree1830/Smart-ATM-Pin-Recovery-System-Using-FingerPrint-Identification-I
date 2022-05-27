from flask import Flask,render_template,request
import random
import serial
import threading
import json

st = 0
otpValue = 0
id = 0

com = 0
ser = 0
for i in range(0,256):
    try:
        ser = serial.Serial('COM'+str(i),9600)
        print('COM'+str(i))
        com = 'COM'+str(i)
    except :
        print("error")
        pass

print(com)