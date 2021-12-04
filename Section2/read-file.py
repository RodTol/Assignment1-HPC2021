from matplotlib import use
import matplotlib.pyplot as plt
import pandas as pd
lines = []
times = []

print('Inserisci numero del job')

file = input() 
f = open('script_tolloi.sh.o{}'.format(file), 'r')
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
            for j in range(-1,31): 
                f.write(lines[i+j])
            f.close()        
            break
    df = pd.read_fwf("{}.txt".format(k))
    df.to_csv('{}.csv'.format(k), index = None)
    continue

col = ['#bytes', 't[usec]']

for k in opzioni :   
    df = pd.read_csv('{}.csv'.format(k), usecols=col)
    #print(df)
    plt.figure()
    plt.yscale("log")
    plt.scatter(df.iloc[:,0], df.iloc[:,1])
    plt.savefig('{}.png'.format(k))
    

