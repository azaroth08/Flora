import csv
import time
for i in range(50):
    l=[]
    while(1):
        with open(Flora.Lidar) as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=',')
            for row in csv_reader:
                    l.append(row)
        if len(l)>0:
            print(len(l))
            time.sleep(0.1)
            break
        else:
            print(l)
    
