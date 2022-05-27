from flask import Flask,render_template,request
import random
import serial
import threading
import json
import time

#############################################################################
st = 0
otpValue = 0
id = 0
com = 0
ser = 0
pin_count = 0
f = open("db.json")
dct = json.load(f)
print(dct)
f.close()
for i in range(0,256):
    try:
        ser = serial.Serial('COM'+str(i),9600,timeout=2)
        print('COM'+str(i))
        com = 'COM'+str(i)
    except :
        pass
        #print("Connect the device")
print(com)


#############################################################################


def getCard(): 
    global ser
    ser.write(b"getcardid")
    r = 0
    while 1:
        r = ser.readline()
        try:
            if len(r) > 0 :
                r = r.decode('utf-8')
                print(r)
                if 'CardId :' in r:
                    r = r.split("CardId :")[1].split(",")[0]
                    print(r)
                    break
        except:
            print("Error card")
            
            pass
    return r
#############################################################################   
def getindex():
    global ser
    ser.write(b"getfingerindex")
    r = 0
    while 1:
        r = ser.readline()
        try:
            if len(r) > 0:
                r = int(r.decode('utf-8').split(":")[1])
                print(r)
                break
        except:
            print("Error card")
            pass
    return r

#############################################################################
def getsendSMS(otpnum,mobile):
    global ser
    otpnum = f"otp:{otpnum},mobile:{mobile};".encode("utf-8")#otp:1234,mobile:7095797212;
    #otp:1234,mobile:1234567890;
    print("sending : ",otpnum)
    ser.write(otpnum)
    r = 0
    while 1:
        r = ser.readline()
        if len(r) > 0 :
            print(r)
            break
    return r.decode('utf-8')
#############################################################################
def load(pinValue):
    global id
    print(id)
    f = open("db.json")
    dct = json.load(f)
    dct[id]["pin"] = pinValue
    print(dct)
    f.close()
    with open('db.json', 'w') as json_file:
        json.dump(dct, json_file)
#############################################################################
def get_w():
    global ser
    global st
    global otpValue
    global id
    global dct
    while 1:
        if st ==1:
            id = getCard()
            print(dct)
            if(id in dct.keys()):
                st = 2
                continue
                st = 2
                index = getindex()
                print(index, dct[id])
                if dct[id]["index"] == index:
                    print("Authenticated")
                    st = 3
                    otpValue = random.randint(1000,9999)
                    print("OTP : ",otpValue)
                    getsendSMS(otpValue)
                else:st = 0
        elif st == 3:
            index = getindex()
            print(index, dct[id])
            if dct[id]["index"] == index:
                print("Authenticated")
                st = 4
                otpValue = random.randint(1000,9999)
                print("OTP : ",otpValue)
                getsendSMS(otpValue,dct[id]["mobile"])
            else:st = 5
        else:pass
            

#############################################################################
app = Flask(__name__)
@app.route('/',methods=["get","post"])
def home():
    global st
    if request.method=="GET":
        st = 1
        return render_template('home.html')
    if request.method=="POST":
            return {"data":st}
@app.route('/check',methods=["get","post"])
def checkFinger():
    global st
    if request.method=="GET":
        st = 3
        return render_template('check.html')
    if request.method=="POST":
            return {"data":st}
@app.route('/checkpin',methods=["get","post"])
def checkpin():
    global st
    global dct
    global id
    global pin_count
    if request.method=="GET":
        pin_count = 0
        return render_template('checkPin.html')
    if request.method=="POST":
        if st == 2:
            pin_count +=1
            print(dct[id]['pin'],int(request.data.decode("utf-8")))
            if(dct[id]['pin'] == request.data.decode("utf-8")):return {"data":4}
            if pin_count ==4:return {"data":5}
            return {"data":pin_count}
        return {"data":0}
@app.route('/otp',methods=["get","post"])
def otp():
    global st
    global otpValue
    if request.method=="GET":
        st = 0
        return render_template('otpcheck.html')
    if request.method=="POST":
        try:
            otp = int(request.data.decode("utf-8"))
            print('Got OTP :',otp)
            print("Generated OTP :",otpValue)
            if otp == otpValue:return {"data":1}
            else :return {"data":0}
        except Exception as e:
            print(e)
            return {"data":0}
    return ""
@app.route('/change',methods=["get","post"])
def change():
    global dct
    global random_value
    if request.method=="GET":
        return render_template('change.html')
    if request.method=="POST":
        try:
            pin = request.data.decode("utf-8")
            print('Got pin :',pin)
            pin = pin.split(",")[0]
            load(pin)
            f = open("db.json")
            dct = json.load(f)
            print(dct)
            f.close()
        except Exception as e:
            print(e)
    return ""
kwargs = {'host': '0.0.0.0', 'port': 8080, 'threaded': True, 'use_reloader': False, 'debug': True}
if __name__ == '__main__':
    threading.Thread(target=get_w).start()
    threading.Thread(target=app.run, daemon=True, kwargs=kwargs).start()    
