from typing import NewType
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
f.close()


opzioni = ["Serial","Same-socket-4-cores", "Same-socket-8-cores",
    "Same-socket-12-cores", "Two-sockets-4-cores", "Two-sockets-8-cores",
    "Two-nodes-12-cores", "Two-nodes-24-cores", "Two-nodes-48-cores"]

i=0
grid = []
for k in opzioni :
    f = open("Grafici/{}.txt".format(k), "w")
    for line in lines[i:len(lines)]:
        i = i+1
        if '#   0 Process grid ' in line:
            grid.append(lines[i-1][21:len(lines[i-1])])
        if  '# StartResidual    0.00000000000    ' in line:
            for j in range(0,11): 
                f.write(lines[i+j][46:])
            f.close()        
            break
    df = pd.read_fwf("Grafici/{}.txt".format(k))
    df.to_csv('Grafici/{}.csv'.format(k), index = None)
    continue
    
