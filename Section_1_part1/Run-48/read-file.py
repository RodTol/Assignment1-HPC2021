lines = []
times = []
print('Tempi')
for i in range(2,49):
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
plt.scatter(range(2,49),times)
plt.savefig('scalability_plot-48.png')