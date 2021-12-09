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

opzioni = ['Matrice-2400-100-100', 'Matrice-1200-200-100', 'Matrice-800-300-100']

i = 0
a = []

for line in lines:
    if 'Matrice 2400 100 100 ' in line :
        a.append(i)
    if 'Matrice 1200 200 100 ' in line :
        a.append(i)
    if 'Matrice 800 300 100 ' in line :
        a.append(i)
    i+=1
print(a)

i = 0

for k in [0,1,2] :
    f = open("{}.txt".format(opzioni[k]), "w")

    for line in lines[i:a[k]]:
        i = i+1
        if  'Time of comunication+operation is :' in line: 
            f.write(lines[i-5])
            f.write(lines[i])

    f.close()
    #df = pd.read_fwf("{}.txt".format(k))
    #df.to_csv('{}.csv'.format(k), index = None)
    continue

