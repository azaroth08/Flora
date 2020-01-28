from vehicles import *
Flora =Flora('','','')
from Methods import *
scan = Flora.scan()
scanf = ScanOffset(Flora,scan)
x,y = ScanXY(Flora,scanf,Offset=0)
import matplotlib.pyplot as plt
#plt.plot(x,y)

plt.show()