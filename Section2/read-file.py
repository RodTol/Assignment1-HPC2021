import matplotlib.pyplot as plt
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
k=0

f = open("{}.csv".format(opzioni[k]), "w")
for line in lines:
    i+=1
    if  '       #bytes #repetitions      t[usec]   Mbytes/sec' in line:
        for j in range(0,25): 
            f.write(lines[i-1+j])
        f.close()
        k+=1
    

