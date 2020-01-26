'''
Main Vehicle code

All distances in mm
All angles in rad

'''
import math
from math import pi,degrees,radians
from numpy import sign
import serial
#from Lidar import Lidar
from Methods import CartesianPolar
from breezyslam.algorithms import RMHC_SLAM

class Flora(object):
    def __init__(self, Arduino1,Arduino2,Lidar,WheelRadius = 6.1925, HalfAxleLength = 14.25):
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
        self.Arduino1               = serial.Serial(Arduino1,baudrate=9600) #Arduino1 = Drive
        #self.Arduino2               = serial.Serial(Arduino2,baudrate=9600) #Arduino2 = Arm
        self.Lidar                  = "/home/pi/LidarData/LD"
        # Initialize BreZZySlammms
        #self.slam                   = RMHC_SLAM(LaserModel(), MAP_SIZE_PIXELS, MAP_SIZE_METERS)
        self.trajectory             = []
        #self.mapbytes               = bytearray(MAP_SIZE_PIXELS * MAP_SIZE_PIXELS)
        self.previous_distances     = None
        self.previous_angles        = None
    def __str__(self):
        return '<Wheel radius=%f mm Half axle Length=%f mm>' % \
        (self.WR, self.HA)
        
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
       d,theta = scan()
       return d,theta  
   
    def getIRangle(self):       
        return angle
    
    def ArduinoD(self):
        pass
##  ArduinoMedley
    def Send(self,obj,cmd):
        #Send Serial Commands to arduino
        print(cmd)
        obj
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
        self.Move(0,n,s)
        
    def Move(self,a,n,s):
        Flag = 1
        self.Send(self.Arduino1,str(a)+','+str(n)+','+str(s))
        #while Flag:
            #Flag = query(Flag)
            #pause(0.01)
        #return Flag
    
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
        