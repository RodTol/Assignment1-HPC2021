lines = []
times = []
print('Tempi')
for i in range(2,25):
    f = open('out.{}'.format(i), 'r')
    lines = f.readlines()
    f.close

    for line in lines:
        if 'The mean of time is' in line:
            for i in range(0,len(line)): 
                if (line[i] == ':') :
                    tag = i
            print(line[tag+1:len(line)])
            times.append(line[tag+1:len(line)])
import matplotlib.pyplot as plt

#Tc_1 = float(times[22])/float(times[0])*2/22
#Tc_2 = float(times[45])/float(times[0])*2/45

#print(Tc_1)
#print(Tc_2)

plt.scatter(range(len(times)),times)
plt.savefig('time_plot-24.png')
plt.close()


for i in range(len(times)):
    plt.scatter(i+2, float(times[i])/float(times[0]), c='black')

#plt.plot([24,24],[1,125])
#plt.plot([2,48],[2*Tc_1/2,48*Tc_1/2])
plt.savefig('scalability_plot-24.png')