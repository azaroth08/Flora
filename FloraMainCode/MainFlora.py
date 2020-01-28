# -*- coding: utf-8 -*-
"""
Created on Sat Jan 18 11:41:10 2020

@author: Jonathan

Flora Main Script

"""

import vehicles
#import sensors
from algorithms import *
from breezyslam.algorithms import RMHC_SLAM
#from breezyslam.sensors import YDLidarX2 as LaserModel
#import Lidar
#from roboviz import MapVisualizer
import Navigation
from Methods import *
from operator import itemgetter
from math import pi
import time
import pandas as pd
from SimpleMap import *
wheelRadius_mm = 100
halfAxleLength_mm = 150
MAP_SIZE_PIXELS   = 500
MAP_SIZE_METERS   = 10
Lidar_Port        = '/dev/ttyUSB0'
Arduino1_Port     = '/dev/ttyACM0'
Arduino2_Port     = '/dev/ttyACM1'
MIN_SAMPLES   = 200


def Exploration(Flora):
##    data=[]
##    L  = [2,1,2,1];
##    for i in range(len(L)):
##        for j in range(2):
##            Flora.drive(L[i]/2)
##            time.sleep(1)
##            tmp=Flora.scan()
##            data.append(tmp)
##            tmp.to_csv('scans/LD'+str(i)+str(j)+'.csv')
##        Flora.rotate(pi/2)
##        time.sleep(1)
##        tmp=Flora.scan()
##        data.append(tmp)
##        tmp.to_csv('scans/LD_T_'+str(i)+str(j)+'.csv')
            
            
    data=[]
    L  = [2,1,2,1];
    for i in range(len(L)):
        for j in range(2):
            Flora.drive(L[i]/2)
            time.sleep(1)
            tmp=Flora.scan()
            data.append(tmp)
        Flora.rotate(pi/2)
    
    return data

def PlantFindIR(Flora):
    Flag = AlignIR(Flora)
    while(1):
        time.sleep(0.2)
        Flag = AlignIR(Flora)
        if Flag==0: # OUT OF RANGE
            F = 0
            break
        scan = Flora.scan()
        #scanOff =ScanOffset(Flora,scan)
        ScanFilt = FilterPoints(scan,-0.0872*2,0.0872*2)
        DistFromPlant=min(ScanFilt.d)
        print(DistFromPlant)
        if DistFromPlant <= 0.3+0.35/2:
            #theta = FindPlantCenter(Flora.scan())
            # Extrapolate From point cloud if Flora is within Allowed distance
            #if theta > x:
            #    Flora.rotate(theta)
            #Flora.drive(d)
            Flora.Water()
            F = 1
            break
        D = DistFromPlant/2
        Flora.drive(min(D,1))
    #NtxPlant    
    return F

def AlignIR(Flora):
    theta = Flora.getIRangle()
    Flag = 1
    for i in range(2):
        if theta ==-1:
            theta = pi
        Flora.rotate(theta)
        time.sleep(0.1)
        theta = Flora.getIRangle()
        if theta == 0:
            break
        elif theta == -1:
            Flag = 0
            break
    return Flag
    
        
def HomeDrive(Flora):
    return 1

def Park(Flora):
    return 1

def Water(Flora):
    return 1

def PlantSearch(Flora):
    return 1

def MapNeeded(Flora):
    return 1

def Nav(Flora,gx,gy):
    goal = 0
    while (not goal):
        tmp=Flora.pose()
        A = ConvertMapToScatter(Flora)
        ox,oy
        for i in range(10):    
            rx,ry=Navigation.NAV(tmp[0],tmp[1],gx,gy,ox,oy,flora.rad,1,i*20)
            if len(rx) != 0:
                break
        for i in range(len(rx)):
            Flag = Flora.drivexy(rx,ry)
            if Flag == -1:  #OBSTACLE
                UpdateMapForObstacle(Flora)
                break
        goal = 1
    return rx,ry

def ExtractMapFeatures(Flora):    
    class Points:
        pass
    Points.Room = [] # Center of room
    Points.Waypoints = [] # Pts 
    return R,Points

def Parking(Flora):
    return 1

def ImportMap(Flora):
    return 1

def Refill(Flora):
    return 1

def mm2pix(mm):
    return int(mm / (MAP_SIZE_METERS * 1000. / MAP_SIZE_PIXELS))
  
def pix2mm(pix):
    return int(pix*(MAP_SIZE_METERS * 1000. / MAP_SIZE_PIXELS))

if __name__ == '__main__':
    #Initialize ROBOT and LIDAR
    Flora = vehicles.Flora(Arduino1_Port,Arduino2_Port,Lidar_Port)
    lidar=Lidar.yd(Flora.Lidar)
    # Leave Home Base
    Flora.drive(self,0,0.5)
    #Check for map/ Explore if needed
    if MapNeeded():
        Exploration(Flora)
    else:
        ImportMap(Flora)
    R,Points =ExtractMapFeatures(Flora)
    for i in range(R):                                  # Loop through Rooms
        if len(Flora.Channels)==0:
            break;
        Nav(Flora,Points.Room[i][0],Points.Room[i][1])  
        for j in Flora.Channels:                        # Loop through plants remaining List of Channel#
            ch = Flora.RFScan(j)                        # Scan Channels
            if ch == 2:                                 # If channel j is not in range go to nxt
                continue
            elif ch == 0:                               # Plant is already watered
                Flora.Channels[j] =[]
            else:                                                 # Plant needs to be watered go find it
                for k in range(len(Points.Waypoints[j])):         # Loop through Points to be within IR range
                    F = PlantFindIR(Flora)
                    if F == 1:
        # if F ==1 this means Flora found and watered the plant if its = 0 it was out of IR range
                        Flora.Channels[j] = []
        ### Nxt Iteration it scans rf from plant, which might not work -> verify rf range
                        continue
                    else:
                        Nav(Flora,Points.Waypoints[j][k][0],Points.Waypoints[j][k][1])
        Flora.Channels =list(filter(([]).__ne__, Flora.Channels))
        
    Nav(Flora,Flora.InitPose[0],Flora.InitPose[1]) # Return Close to Home
    P = Parking(Flora)
    if P:
        Refill(Flora) 
### Points.Waypoints Format = [ [ [x,y],[x,y] ] , [Room2] , [Room3] ]
        
        