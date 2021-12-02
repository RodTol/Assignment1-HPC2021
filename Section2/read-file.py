import matplotlib.pyplot as plt
import pandas as pd
lines = []
times = []
print('Tempi')
f = open('script_tolloi.sh.o41040', 'r')
lines = f.readlines()
f.close

opzioni = ['by-core-infiniband', 'by-socket-infiniband',
        'by-node-infiniband', 'by-core-ethernet', 'by-socket-ethernet',
        'by-node-ethernet']

i=0
for k in opzioni :
    f = open("{}.txt".format(k), "w")
    for line in lines[i:len(lines)]:
        i = i+1
        if  '       #bytes #repetitions      t[usec]   Mbytes/sec' in line:
            for j in range(0,25): 
                f.write(lines[i+j])
            f.close()        
            break
    df = pd.read_fwf('{}.txt'.format(k))
    df.to_csv('{}.csv'.format(k), index = None)
    continue
    

    

