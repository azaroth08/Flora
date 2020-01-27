from Methods import *
from LidarClass import *
from breezyslam.algorithms import RMHC_SLAM
import pandas as pd
from numpy import degrees
import matplotlib.pyplot as plt
Laser =YDlidarx2()
slam2 = RMHC_SLAM(Laser,1000,40)
mapbytes = bytearray(1000*1000)
class Flora:
    pass
Flora.MSP = 1000
P=[(0,0,0),(1000,0,1)]
for i in range(2):
    a=str(i)
    ld=pd.read_csv('/home/pi/Flora/FloraMainCode/scans/LD0'+a+'.csv')
    tmpd=ld.range*1000
    tmpa=degrees(ld.angle)
    slam2.update(tmpd.tolist(),scan_angles_degrees=tmpa.tolist())
    slam2.getmap(mapbytes)
    Flora.mapbytes=mapbytes
    #slam2.position.x_mm = 10000+1000
AA=ByteMapToNP(Flora)
plt.imshow(AA)
plt.show()