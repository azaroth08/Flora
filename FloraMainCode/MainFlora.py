# -*- coding: utf-8 -*-
"""
Created on Sat Jan 18 11:41:10 2020

@author: Jonathan

Flora Main Script

"""

import vehicles
import sensors
from algorithms import *
from breezyslam.algorithms import RMHC_SLAM
from breezyslam.sensors import YDLidarX2 as LaserModel
import Lidar
from roboviz import MapVisualizer
import Navigation
from Methods import *
from operator import itemgetter
from math import pi
wheelRadius_mm = 100
halfAxleLength_mm = 150
MAP_SIZE_PIXELS   = 500
MAP_SIZE_METERS   = 10
Lidar_Port        = '/dev/ttyUSB0'
Arduino1_Port     = '/dev/ttyACM0'
Arduino2_Port     = '/dev/ttyACM1'
MIN_SAMPLES   = 200


def Exploration(Flora):
    L  = [[0,2],[1,2],[1,0],[0,0]];
    Flora.drive(2)
    Flora.rotate(pi/2)
    Flora.drive(1)
    Flora.rotate(pi/2)
    Flora.drive(2)
    Flora.rotate(pi)
    return 1

def PlantFindIR(Flora):
    theta = Flora.getIRangle()
    Flora.rotate(theta)
    while(1):
        Scan = Flora.scan()
        d,theta = FilterPoints(Scan,-5,5)
        if min(d) <= 500:
            theta = FindPlantCenter(Flora.scan())
            # Extrapolate From point cloud if Flora is within Allowed distance
            if theta > x:
                Flora.rotate(Xamount)
            Flora.drive(d)
        D = min(d)/2
        Flora.drive(D)
    return 1

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
        Nav(Flora,Points.Room[i][0],Points.Room[i][1])  
        for j in Flora.RemainingChannels:        # Loop through plants remaining List of Channel#
            ch = RFScan(Flora,j)                        # Scan Channels
            if ch == -1:                            # If no chanel is not in range go to nxt
                break
            for k in range(len(Points.Waypoints[j])):         # Loop through Points to be within IR range
                F = IRScan(Flora)
                if F == 1:
                    PlantFindIR(Flora)                  # Find the Plant
                    Water(Flora)                         # Water the Plant
### Nxt Iteration it scans rf from plant, which might not work -> verify IR range
                    break
                else:
                    Nav(Flora,Points.Waypoints[j][k][0],Points.Waypoints[j][k][1])
    Nav(Flora,Flora.InitPose[0],Flora.InitPose[1]) # Return Close to Home
    P = Parking(Flora)
    if P:
        Refill(Flora) 
### Points.Waypoints Format = [ [ [x,y],[x,y] ] , [Room2] , [Room3] ]
        
        