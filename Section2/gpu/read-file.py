from matplotlib import use
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from math import sqrt

lines = []
times = []

print('Inserisci numero del job')

file = input() 
f = open('script_tolloi_gpu.sh.o{}'.format(file), 'r')
lines = f.readlines()
f.close


opzioni = ['mpi-by-core-infiniband', 'mpi-by-socket-infiniband',
        'mpi-by-node-infiniband', 'mpi-by-core-ethernet', 'mpi-by-socket-ethernet',
        'mpi-by-node-ethernet', 'intel-by-core-infiniband', 'intel-by-socket-infiniband',
        'intel-by-node-infiniband']
        #, 'intel-by-core-ethernet', 'intel-by-socket-ethernet','intel-by-node-ethernet'

i=0
for k in opzioni :
    f = open("Grafici/{}.txt".format(k), "w")
    for line in lines[i:len(lines)]:
        i = i+1
        if  '       #bytes #repetitions      t[usec]   Mbytes/sec' in line:
            for j in range(-1,31): 
                f.write(lines[i+j])
            f.close()        
            break
    df = pd.read_fwf("Grafici/{}.txt".format(k))
    df.to_csv('Grafici/{}.csv'.format(k), index = None)
    continue

col = ['#bytes', 't[usec]']
x = np.linspace(0,536870912, 1000000)

lamb = [0.2001, 
        0.4106,
        0.9873,
        4.879,
        7.536,
        15.30,
        0.4106,
        0.4332,
        1.126]
b = [6425.844,
    5645.615,
    12053.12,
    5331.776404,
    3345.158,
    2520.641,
    5645.615,
    4281.702,
    12271.34]

ind = 0
for k in opzioni :   
    err = []
    y = []

    plt.rcParams["figure.autolayout"] = True
    df = pd.read_csv('Grafici/{}.csv'.format(k), usecols=col)

    plt.figure()
    y = lamb[ind] + x/b[ind]

    plt.yscale("log")
    plt.plot(x,y)
    plt.scatter(df.iloc[:,0], df.iloc[:,1])
    plt.savefig('Grafici/{}.png'.format(k))

    err = (df.iloc[:,1] - lamb[ind] + df.iloc[:,0]/b[ind])**2
    err = sqrt(sum(err)/len(df.iloc[:,1]))
    print(err)

    ind +=1
    del(y)
    

