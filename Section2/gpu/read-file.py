from os import name, read, sep
from matplotlib import use
import matplotlib.pyplot as plt
import pandas as pd
from math import sqrt

from pandas.io.parsers import read_csv
import numpy as np
from scipy.optimize import least_squares

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

lamb = [0.2001, 
        0.4106,
        0.9873,
        4.879,
        7.536,
        15.30,
        0.2350,
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

lamb = []
b = []
print("STIMA B --------------")
n_val = 6
#Faccio l' LSM per le b
for k in opzioni:
    df = pd.read_csv('Grafici/{}.csv'.format(k), usecols=col) #apro i file in un dataframe
    x = df["#bytes"].to_numpy() # i miei bytes
    x = x[len(x)-n_val:]
    A = np.vstack([x, np.ones(len(x))]).T # creo la matrice fatta dalle coppie dei valori e di 1
    y = df["t[usec]"].to_numpy() # i miei tempi in microsecondi
    y = y[len(y)-n_val:]

    x0 = np.array([0.01,0])

    def fun(par, x, y):
        return par[0] + x/par[1] - y 

    lst = least_squares(fun, x0, args=(x, y), bounds = ([0, 0],[np.inf, np.inf]))

    print(lst.x[1], "mb/s", k)  #usec e mb/s
    b.append(lst.x[1]) # bytes/usec è uguale a mb/sec

print("STIMA LAMBDA --------------")
n_val = 6
#Faccio l' LSM e per le lambda usando i primi 8 valori
for k in opzioni:
    df = pd.read_csv('Grafici/{}.csv'.format(k), usecols=col) #apro i file in un dataframe
    x = df["#bytes"].to_numpy() # i miei bytes
    x = x[0:n_val]
    A = np.vstack([x, np.ones(len(x))]).T # creo la matrice fatta dalle coppie dei valori e di 1
    y = df["t[usec]"].to_numpy() # i miei tempi in microsecondi
    y = y[0:n_val]

    x0 = np.array([0.01,0])

    def fun(par, x, y):
        return par[0] + x/par[1] - y 

    lst = least_squares(fun, x0, args=(x, y), bounds = ([0, 0],[np.inf, np.inf]))

    print(lst.x[0], " usec", k)  #usec e mb/s
    lamb.append(lst.x[0])  #usec

x = np.linspace(0, 536870912, 1000000,  dtype = int)

#Faccio i grafici usando i paremetri
ind = 0 
for k in opzioni :   
    err = []
    y = []

    plt.rcParams["figure.autolayout"] = True
    df = pd.read_csv('Grafici/{}.csv'.format(k), usecols=col)

    plt.figure()
    y = lamb[ind] + x/b[ind]
    #print("Grafici\n", lamb[ind], b[ind], k)

    plt.yscale("log")
    plt.plot(x,y)
    plt.scatter(df.iloc[:,0], df.iloc[:,1])
    plt.savefig('Grafici/{}.png'.format(k))

    #err = (df.iloc[:,1] - lamb[ind] + df.iloc[:,0]/b[ind])**2
    #err = sqrt(sum(err)/len(df.iloc[:,1]))
    #print(err)

    ind +=1
    del(y)

print("vuoi lo zoom? y/n")
resp = input()

if resp == 'y':
    #zoomiamo sui primi
    x = np.linspace(0, 0.2e8, 1000000)
    ind = 0 
    for k in opzioni :   
        err = []
        y = []

        plt.rcParams["figure.autolayout"] = True
        df = pd.read_csv('Grafici/{}.csv'.format(k), usecols=col)

        plt.figure()
        y = lamb[ind] + x/b[ind]
        #print("Grafici\n", lamb[ind], b[ind], k)

        #plt.yscale("log")
        plt.plot(x,y)
        plt.scatter(df.iloc[1:24,0], df.iloc[1:24,1])
        plt.savefig('Grafici/{}.png'.format(k))

        #err = (df.iloc[:,1] - lamb[ind] + df.iloc[:,0]/b[ind])**2
        #err = sqrt(sum(err)/len(df.iloc[:,1]))
        #print(err)

        ind +=1
        del(y)
#Faccio una tabella con le stime
table = []
from tabulate import tabulate
for i in range(len(lamb)):
    a = [lamb[i], b[i], opzioni[i]]
    table.append(a)
f = open("table.txt", "w+")
a = tabulate(table, headers = ['Latency [usec]', 'bandwidth [MB/s]', ''], tablefmt='fancy_grid', showindex=True)
f.write(a)
f.close()

#Calcoli i tempi computed
add = []
for k in range(len(opzioni)):
    comp = []
    df = pd.read_csv('Grafici/{}.csv'.format(opzioni[k]), usecols=col) #apro i file in un dataframe
    x = df["#bytes"].to_numpy() # i miei bytes
    for i in x:
        t = lamb[k] + i/b[k]
        band = i/t
        comp.append([t,band])

    add.append(comp)       

#Scrivo i CSV finali corretti
col_names = ["#bytes", "#repetitions", "#t[usec]", "Mbytes/sec"]
command_line = ["mpirun -np 2 --map-by core  --report-bindings ./IMB-MPI1 PingPong -msglog 29",
                "mpirun -np 2 --map-by socket  --report-bindings ./IMB-MPI1 PingPong -msglog 29",
                "mpirun -np 2 --map-by node --report-bindings ./IMB-MPI1 PingPong -msglog 29",
                "mpirun -np 2 --mca pml ob1 --report-bindings  --map-by core --mca btl tcp,self ./IMB-MPI1 PingPong -msglog 29",
                "mpirun -np 2 --mca pml ob1 --report-bindings  --map-by socket --mca btl tcp,self ./IMB-MPI1 PingPong -msglog 29",
                "mpirun -np 2 --mca pml ob1 --report-bindings  --map-by node --mca btl tcp,self ./IMB-MPI1 PingPong -msglog 29",
                "mpirun -np 2 -ppn=2 -env I_MPI_DEBUG 5 -genv I_MPI_PIN_PROCESSOR_LIST 0,2  ./IMB-MPI1 PingPong -msglog 29",
                "mpirun -np 2 -ppn=2 -env I_MPI_DEBUG 5 -genv I_MPI_PIN_PROCESSOR_LIST 0,1  ./IMB-MPI1 PingPong -msglog 29",
                "mpirun -np 2 -ppn=1 -env I_MPI_DEBUG 5 -host ct1pt-tnode007,ct1pt-tnode009 ./IMB-MPI1 PingPong -msglog 29"]

for k in range(9):
    f = pd.read_csv("Grafici/{}.csv".format(opzioni[k]), skiprows=[0], names=col_names )
    vett_1 = []
    vett_2 = []
    for i in range(31):
        vett_1.append(add[k][i][0])
        vett_2.append(add[k][i][1])
    f['t_comp'] = vett_1
    f['band_comp'] = vett_2
    f.to_csv('CSV/{}.csv'.format(opzioni[k]), index=None, header=None)

    with open('CSV/{}.csv'.format(opzioni[k]), 'r+') as g:
        #metti if per mettere solo un nodo nella header line 2
        content = g.read()
        g.seek(0, 0)
        if k == 2 or k==5 or k == 8:
            g.write('#header_line 1:{}'.format(command_line[k]) + '\n' +
             "#header_line 2:  ct1pt-tnode007,ct1pt-tnode009 " + '\n' +
             "#header_line 3: " + str(table[k][0]) + " " + str(table[k][1])  +'\n' + 
             "#header: #bytes, #repetitions, t[usec], Mbytes/sec, t[usec] computed , Mbytes/sec (computed)\n" + content)

        else:
            g.write('#header_line 1:{}'.format(command_line[k]) + '\n' +
             "#header_line 2:  ct1pt-tnode007 " + '\n' +
             "#header_line 3: " + str(table[k][0]) + " " + str(table[k][1])  +'\n' +   
             "#header: #bytes, #repetitions, t[usec], Mbytes/sec, t[usec] computed , Mbytes/sec (computed)\n" + content)

col_names = ['#bytes', '#repetitions', 't[usec]', 'Mbytes/sec', 't[usec] computed' , 'Mbytes/sec (computed)']
for k in range(len(opzioni)):
    df = pd.read_csv('CSV/{}.csv'.format(opzioni[k]),  skiprows=[0,1,2])
    print(df)