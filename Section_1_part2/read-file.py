from matplotlib import use
import matplotlib.pyplot as plt
import pandas as pd

lines = []
times = []

topolo = ["24, 1, 1" , "2, 12, 1" ,
        "3, 8, 1" ,
        "4,6,1" ,
        "6,4,1" ,
        "8,3,1" ,
        "12, 2, 1" ,
        "1,24,1" ,
        "24,1,1" ,
        "2,2,6" ,
        "2,6,2" ,
        "6,2,2" ,
        "2,3,4" ,
        "2,4,3" ,
        "3,2,4" ,
        "3,4,2" ,
        "4,2,3" ,
        "4,3,2" ,
        "1,1,24",
        "1,24,1" ,
        "24,1,1" ,
        "1,2, 12" ,
        "1,12,2" ,
        "2,1,12" ,
        "2,12,1" ,
        "12,1,2" ,
        "12,2,1" ,
        "1,3,8" ,
        "1,8,3" ,
        "3,1,8" ,
        "3,8,1" ,
        "8,1,3" ,
        "8,3,1" ,
        "1,4,6" ,
        "1,6,4" ,
        "4,1,6" ,
        "4,6,1" ,
        "6,1,4" ,
        "6,4,1"
        ]    

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

a.append(len(lines))
print(a)

i = 0

for k in [0,1,2] :
    f = open("{}.txt".format(opzioni[k]), "w")

    for line in lines[a[k]:a[k+1]]:
        if  'Time of comunication+operation is :' in line: 
            f.write(lines[i-4])
            f.write(lines[i][35:])
        i = i+1

    f.close()
    continue


import plotly.graph_objects as go

                #prima     seconda     terza
media = []  # 1d,2d,3d    1d,2d,3d    1d,2d,3d
for k in opzioni:
    tempi = []
    f = open("{}.txt".format(k), "r")
    lines = f.readlines()
    for i in range(1,40):
        tempi.append(float(lines[(i*2)-1]))
    f.close()

    print(len(tempi))
    print(tempi[len(tempi)-1])

    tempi_2d = [tempi[i] for i in range(1,9)]
    tempi_3d = [tempi[i] for i in range(9,39)]

    media.append(tempi[0]) #media 1d
    media.append(sum(tempi_2d)/8.0) #media2d
    media.append(sum(tempi_3d)/30.0)   #media 3d 

    fig = go.Figure(data=[go.Table(header=dict(values=['Distr', '800 300 100']),
                 cells=dict(values=[topolo, tempi]))
                     ])

    plt.scatter(range(39), tempi, label = '{}'.format(k))

fig.show()
    
#plt.plot([1,39],[sum(media)/3.0,sum(media)/3.0])
plt.plot([0,0.5],[media[0],media[0]], c = 'blue')
plt.plot([0.5,8.5],[media[1],media[1]], c = 'blue')
plt.plot([8.5,38],[media[2],media[2]], c = 'blue')

plt.plot([0,0.5],[media[3],media[3]], c = 'orange')
plt.plot([0.5,8.5],[media[4],media[4]], c = 'orange')
plt.plot([8.5,38],[media[5],media[5]], c = 'orange')

plt.plot([0,0.5],[media[6],media[6]], c = 'green')
plt.plot([0.5,8.5],[media[7],media[7]], c = 'green')
plt.plot([8.5,38],[media[8],media[8]], c = 'green')

plt.plot([0.5,0.5],[0.19,0.29], c = 'black')
plt.plot([8.5,8.5],[0.19,0.29], c = 'black')
plt.legend()
plt.grid(True)
plt.savefig('plot.png')
plt.close


