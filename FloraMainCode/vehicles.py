'''
Main Vehicle code

All distances in mm
All angles in rad

'''
import smbus
import math
from math import pi,degrees,radians
from numpy import sign
import serial
#from Lidar import Lidar
from Methods import CartesianPolar
from breezyslam.algorithms import RMHC_SLAM
import time
import pandas as pd
import numpy as np
class Flora(object):
    def __init__(self, Arduino1='',Arduino2='',Lidar='',WheelRadius = 6.1925/100, HalfAxleLength = 14.25/100):
        Wheel =  'ACM1'
        # Object Properties
        self.WR = WheelRadius  
        self.HAL = HalfAxleLength
        self.timestampSecondsPrev   = None        
        self.leftWheelDegreesPrev   = None
        self.rightWheelDegreesPrev  = None
        self.Map                    = None
        self.Pose                   = [0,0,0]
        self.PoseEstimate           = [0,0,0]
        #Initialize Communication to Peripherals
        if len(Arduino1)>0:
            self.Arduino1           = serial.Serial(Arduino1,baudrate=9600) #Arduino1 = Drive
        if len(Arduino2)>0:
            self.Arduino2           = serial.Serial(Arduino2,baudrate=9600) #Arduino2 = Arm
        self.Lidar                  = "/home/pi/LidarData/LD"
        # Initialize BreZZySlammms
        #self.slam                   = RMHC_SLAM(LaserModel(), MAP_SIZE_PIXELS, MAP_SIZE_METERS)
        self.trajectory             = []
        #self.mapbytes               = bytearray(MAP_SIZE_PIXELS * MAP_SIZE_PIXELS)
        self.previous_distances     = None
        self.previous_angles        = None
        self.xoff                   = 6.45/100  #m
        self.yoff                   = 8.805/100 #m
        self.channels               = [1,2,3]
    def __str__(self):
        return '<Wheel radius=%f mm Half axle Length=%f mm>' % \
        (self.WR, self.HAL)
        
    def __repr__(self):      
        return self.__str__()

    def drivexy(self,delta_x,delta_y): #Drive to a relative xy point
        alpha = self.Pose[2]
        d,theta = CartesianPolar(delta_x,delta_y)
        sigma = theta - alpha
        self.rotate(sigma)
        self.drive(d)        
        
    def rotate(self,theta): #Rotate Flora on itself
        if theta > pi: 
           theta = theta - 2*pi
        #elif theta < pi:
        #    theta = theta
        if not (theta<0.008 and theta>-0.008):
            Steps=round((abs(theta)/(pi*2))*(self.HAL/self.WR)*200)
            Turn = int(sign(theta)+1)/2
            self.Move(1,Turn,Steps)
            
    def scan(self):
        tmp= pd.read_csv(self.Lidar)
       #scan = tmp.rename(columns={'range': 'd', 'angle':'theta'})
        class scan:
           pass
        dd = []
        ttheta = []
        for i in range(len(tmp.range)):
            if (tmp.range[i]>0.01):
                ttheta.append(tmp.angle[i]);
                dd.append(tmp.range[i])
        scan.d = np.asarray(dd)
        scan.theta = np.asarray(ttheta)
        
        #scan.d = tmp.range.values
        #scan.theta = tmp.angle.values
        
       #d,theta = tmp.range,tmp.angle
        return scan   
   
    def SHITgetIRangle(self):
                
        # I2C channel 1 is connected to the GPIO pins
        channel = 1
        #  MCP4725 defaults to address 0x60
        Sensor_Address = 0x0E
        Heading_Register = 0x04 # 1200Hz
        Strength_Register = 0x05 # 1200Hz

        # Initialize I2C (SMBus)
        bus = smbus.SMBus(channel)
        Heading = bus.read_i2c_block_data(Sensor_Address, Heading_Register, 1)
        Strength = bus.read_i2c_block_data(Sensor_Address, Strength_Register, 1)
        angle = radians((Heading*5-90)%360)
        return angle
    
    def getIRangle(self):
        Heading =255
        angle = -1 # if out of range angle = -1
        for i in range(5): # Try 5 times to get angle
            angle = 0
            self.Send(self.Arduino2,'4')
            Flag= 1
            while(Flag):
                time.sleep(0.2)
                tmp=self.Arduino2.read_all()
                if len(tmp)>0:
                    Flag = 0
            Heading=int(tmp)
            if(Heading != 255):
                angle = radians((Heading*5-90)%360)
                break
        return angle
    
    def Water(self):
        Flag =1
        self.Send(self.Arduino2,'5')
        while(Flag):
            time.sleep(0.2)
            tmp=self.Arduino2.read_all()
            if len(tmp)>0:
                Flag = 0
        return not Flag
    
    def RFScan(self,channel):
        c = string(channel)
        self.Send(self.Arduino2,c)
        Flag=1
        while(Flag):
            time.sleep(0.2)
            tmp=self.Arduino2.read_all()
            if len(tmp)>0:
                Flag = 0        
        RF = int(tmp)
        # Plant Already Watered, RF = 0
        # Plant Needs Water, RF=1
        # Plant Not Found, RF =2
        return RF
##  ArduinoMedley
    def Send(self,obj,cmd):
        #Send Serial Commands to arduino
        print(cmd)
        obj.flushInput()
        obj.flushOutput()
        obj.write(bytes(cmd,'utf-8'))

    def query(self,obj,cmd): #Query Serial Commands to arduino
        obj.flushInput()
        obj.write(cmd)
        A=obj.read()
        return A
##ArduinoFunctions(object):
    def drive(self,d):
        if d < 0:
            n=1
        else:
            n=0
        s = round(200*abs(d)/(self.WR*2*pi))
        RemainingSteps = self.Move(0,n,s)
        if RemainingSteps ==0 :
            Distance = d
        else:
            Distance = d - RemainingSteps*(self.WR*2*pi)/200
        
        return Distance
        
    def Move(self,a,n,s):
        Flag = 1
        self.Send(self.Arduino1,str(a)+','+str(n)+','+str(s))
        while Flag:
            tmp=self.Arduino1.read_all()
            if len(tmp)>0:
                Flag = 0
            time.sleep(0.2)
        return int(tmp)
    
    def Ultrasonic(self,n):
        D = query('u,'+str(n))
        return D
    
    def Odom(self):
        o=query('o')
        return o
    
##ArduinoObj2
    
        
            
        '''
        API:
            Drive Both wheels:
                a,s,n
                a = 0
                s = steps
                n = 1 forward, 0 backwards
            Rotate:
                a,s,n
                a = 1
                s = steps
                n = 1 for right, 0 for left
            Odometry:
                a=2
                get total wheel count
            Ultrasonic:
                Get Ultrasonic sensor reading
                a,n
                a=3
                n= 1 for right sensor, 0 for left sensor
        '''
        