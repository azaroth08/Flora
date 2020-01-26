from Methods import *
from LidarClass import *
from breezyslam.algorithms import RMHC_SLAM
import pandas as pd
from numpy import degrees
import matplotlib.pyplot as plt
Laser =YDlidarx2()
slam2 = RMHC_SLAM(Laser,500,20)
mapbytes = bytearray(500*500)
class Flora:
    pass
Flora.MSP = 500
P=[(0,0,0),(1000,0,1)]
for i in range(2):
    a=str(i+1)
    ld=pd.read_csv('/home/pi/FloraMainCode/LD'+a+'.csv')
    tmpd=ld.range*1000
    tmpa=degrees(ld.angle)
    slam2.update(tmpd.tolist(),scan_angles_degrees=tmpa.tolist())
    slam2.getmap(mapbytes)
    Flora.mapbytes=mapbytes
    slam2.position.x_mm = 10000+1000
AA=ByteMapToNP(Flora)
plt.imshow(AA)
plt.show()