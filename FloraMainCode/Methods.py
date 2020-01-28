# -*- coding: utf-8 -*-
"""
Created on Sat Jan 25 13:43:40 2020

Methods for Flora 

@author: Jonathan
"""


from math import pi,atan,degrees,sqrt,radians
from numpy import sign
import numpy as np
from PIL import Image

def CartesianPolar(x,y):
    #Takes x,y gets angle changes reference frame to lidar -> In rad
    theta = pi-pi/2*(1+sign(x))*(1-sign(y**2))-pi/4*(2+sign(x))*sign(y)-sign(x*y)*atan((abs(x)-abs(y))/(abs(x)+abs(y)))
    theta2 = (-theta+pi/2)%(2*pi)
    d = sqrt(x**2+y**2)
    return d,theta2

def ScanOffset(Flora,scan): #xy are offset from center of Flora
    class newscan:
        pass
    tmpx,tmpy =ScanXY(Flora,scan)
    d = []
    theta = []
    for i in range(len(tmpx)):
        tmp = CartesianPolar(tmpx[i],tmpy[i])
        d.append(tmp[0])
        theta.append(tmp[1])
    newscan.d = np.asarray(d)
    newscan.theta = np.asarray(theta)
    return newscan
        
def ScanXY(Flora,scan,Offset=1):
    'Offset = 1 -> xy from center flora, Offset = 0 -> xy from lidar'
    #x = np.ndarray.tolist(np.sin(scan.theta)*scan.d)
    #y = np.ndarray.tolist(np.cos(scan.theta)*scan.d)
    x = np.sin(scan.theta)*(scan.d)
    y = np.cos(scan.theta)*(scan.d)
    if Offset:
        x = x-Flora.xoff
        y = y-Flora.yoff
    return x,y

def FilterPoints(scan,minD,maxD):
    dd = []
    ttheta = []
    class newscan:
        pass
    for i in range(len(scan.d)):
        if (scan.theta[i] >= minD) and (scan.theta[i] <= maxD):
            ttheta.append(scan.theta[i]);
            dd.append(scan.d[i])
    newscan.d = np.asarray(dd)
    newscan.theta = np.asarray(ttheta)
    return newscan

def ConvertMapToScatter(Flora,T):
    LT = -0.0001
    HT = T
    A=ByteMapToNP(Flora)
    x=[];y=[];
    Fact = Flora.MAP_SIZE_METERS*1000/Flora.MAP_SIZE_PIXELS
    for  i in range(len(A[0])):
        for j in range(len(A[1])):
            if A[i,j] > LT and A[i,j] < HT:
                x.append(i*Fact)
                y.append(j*Fact)
    return x,y

def ByteMapToNP(Flora):
    MSP = Flora.MSP
    mapbytes = Flora.mapbytes
    A = np.asarray(Image.frombuffer('L',(MSP,MSP), mapbytes,'raw','L',0,1),dtype='float32')
    return A

def SLAM(Flora):
    # Extract distances and angles from triples
    distances = scan.distance
    angles    = scan.angle
    ### Pose Is supposed to be a tuple of xy,theta,
    Pose = Flora.Pose 
    # Update SLAM with current Lidar scan and scan angles
    Flora.slam.update(distances, pose_change=Pose,scan_angles_degrees=angles,)
    # Get current robot position
    x_mm,y_mm,theta = Flora.slam.getpos()
    Flora.Pose = [x_mm,y_mm,theta]
    #Get current map bytes as grayscale
    self.slam.getmap(self.mapbytes)
    self.trajectory.append((x_mm, y_mm,theta))
            
    [x2,y2] = Exploration(slam,[x_mm,y_mm,theta])  
    [x3,y3] = Nav(slam,x2,y2)