from matplotlib import use
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from math import sqrt

lines = []
times = []

print('Inserisci numero del job')

file = input() 
f = open('script_tolloi.sh.o{}'.format(file), 'r')
lines = f.readlines()
f.close


opzioni = ["Same-socket-4-cores", "Same-socket-8-cores",
    "Same-socket-12-cores", "Two-sockets-4-cores", "Two-sockets-8-cores",
    "Two-sockets-12-cores"]

i=0
for k in opzioni :
    f = open("Grafici/{}.txt".format(k), "w")
    for line in lines[i:len(lines)]:
        i = i+1
        if  '# StartResidual    0.00000000000    ' in line:
            for j in range(0,10): 
                f.write(lines[i+j][48:])
            f.close()        
            break
    df = pd.read_fwf("Grafici/{}.txt".format(k))
    df.to_csv('Grafici/{}.csv'.format(k), index = None)
    continue
    

medie = []

for k in opzioni :   
    #plt.rcParams["figure.autolayout"] = True
    #plt.figure()

    df = pd.read_csv('Grafici/{}.csv'.format(k), usecols=[0,1], names=['max', 'min'])
    print(df)
    med= ((df['max'])+(df['min']))/2
    medie.append(sum(med)/len(med))

    print(sum(med)/len(med))

plt.scatter([4,8,12],medie[0:3], label="same scoket")
plt.scatter([4,8,12],medie[3:6], label="two sockets")
plt.legend()
plt.savefig('Grafici/scalability.png')