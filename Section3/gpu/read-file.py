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
f.close


opzioni = ["Serial","Same-socket-4-cores", "Same-socket-8-cores",
    "Same-socket-12-cores", "Two-sockets-4-cores", "Two-sockets-8-cores",
    "Two-nodes-12-cores", "Two-nodes-24-cores", "Two-nodes-48-cores"]

i=0
grid = []
for k in opzioni :
    for line in lines[i:len(lines)]:
        i = i+1
        if '#   0 Process grid ' in line:
            grid.append(lines[i-1][21:len(lines[i-1])])
    continue

medie = []
Perf = []
for k in opzioni :   
    df = pd.read_csv('Grafici/{}.csv'.format(k), usecols=[0,1,7], names=['max', 'min','MLUP'])
    print(df)
    for x in df['max']:
        x = float(x)
    for x in df['min']:
        x = float(x)
    for x in df['MLUP']:
        x = float(x)
    med= ((df['max'])+(df['min']))/2
    medie.append(sum(med)/len(med))
    print(sum(med)/len(med))
    
    Perf.append(sum(df['MLUP']/len(df['MLUP'])))

print('Perf\n',Perf)

plt.scatter([1], medie[0], label = 'Serial')
plt.scatter([4,8,12],medie[1:4], label="Same socket")
plt.scatter([4,8,12,24,48],medie[4:9], label="Two sockets")
plt.legend()
plt.savefig('Grafici/medie.png')
plt.close()

lamb = [0.2*10**-6,
        0.413333*10**-6,
        0.996857*10**-6]
b = [4175.48,
    5658.45,
    11998]

P = []
for i in range(1,len(opzioni)):
    Nx = int(grid[i][0])
    Ny = int(grid[i][4])
    Nz = int(grid[i][8])

    k = 0
    process_grid = [Nx, Ny, Nz]
    for num in process_grid :
        if (num > 1) :
            k += 1
    k = k*2
    N=Nx*Ny*Nz

    L = 1200
    print(i)

    ind = 0
    if i == 1 or i ==2 or i==3:
        ind = 0
    elif i == 4 or i ==5 or i==6:
        ind = 1
    elif i == 7 or i ==8 or i==9:
        ind = 2

    c = (L**2)*k*2*8*10**(-6) 
    Tc = (c/b[ind])+(k*lamb[ind]) 

    P.append(((L**3)*N)/(medie[0] + Tc) *10**(-6))

    print(opzioni[i])
    print('c', c)
    print('Tc', Tc)
    print('P', P[i-1])

plt.scatter([4,8,12],P[0:3], label="Same socket-theoretical")
plt.scatter([4,8,12,24,48],P[3:8], label="Two sockets-theoretical")
plt.scatter([4,8,12],Perf[1:4], label="Same socket")
plt.scatter([4,8,12,24,48],Perf[4:9], label="Two sockets")
plt.plot([4, 48], [((L**3)*4/medie[0])*10**(-6), (((L**3)*48)/medie[0])*10**(-6)]) #retta perfect performance
plt.legend()
plt.savefig('Grafici/perf.png')
plt.close()

diff = []
diff.append (((L**3)*4)/medie[0]*10**(-6) - P[0])
diff.append (((L**3)*8)/medie[0]*10**(-6) - P[1])
diff.append (((L**3)*12)/medie[0]*10**(-6) - P[2])

diff.append (((L**3)*4)/medie[0]*10**(-6) - P[3])
diff.append (((L**3)*8)/medie[0]*10**(-6) - P[4])
diff.append (((L**3)*12)/medie[0]*10**(-6) - P[5])

diff.append (((L**3)*24)/medie[0]*10**(-6) - P[6])
diff.append (((L**3)*48)/medie[0]*10**(-6) - P[7])

diff2 = []
diff2.append (((L**3)*4)/medie[0]*10**(-6) - Perf[1])
diff2.append (((L**3)*8)/medie[0]*10**(-6) - Perf[2])
diff2.append (((L**3)*12)/medie[0]*10**(-6) - Perf[3])

diff2.append (((L**3)*4)/medie[0]*10**(-6) - Perf[4])
diff2.append (((L**3)*8)/medie[0]*10**(-6) - Perf[5])
diff2.append (((L**3)*12)/medie[0]*10**(-6) - Perf[6])

diff2.append (((L**3)*24)/medie[0]*10**(-6) - Perf[7])
diff2.append (((L**3)*48)/medie[0]*10**(-6) - Perf[8])

plt.scatter([4,8,12],diff[0:3], label="Same socket-theoretical")
plt.scatter([4,8,12,24,48],diff[3:8], label="Two sockets-theoretical")
plt.scatter([4,8,12],diff2[0:3], label="Same socket")
plt.scatter([4,8,12,24,48],diff2[3:8], label="Two sockets")
plt.legend()
plt.savefig('Grafici/diff.png')
plt.close()