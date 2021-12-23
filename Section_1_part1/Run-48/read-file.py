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
            times.append(line[tag+1:len(line)])

import matplotlib.pyplot as plt

print(len(times))

times_float = [float(i) for i in times]

print("Max min")
print(min(times_float))
print(max(times_float))

lam = 0.4106 #us
lam = lam*(10**(-6)) #s
b = 5645.615 #mb/s
size = 4*(10**-6) #mb
Tc = lam + size/b

plt.scatter(range(2,25),times_float[:23])
plt.plot([2,24],[2*(2*Tc),24*(2*Tc)])
plt.grid(True)
plt.locator_params(axis='y', nbins=5)
plt.savefig('time_plot-24.png')
plt.close()

lam = 0.9873 #us
lam = lam*(10**(-6)) #s
b = 12000.615 #mb/s
size = 4*(10**-6) #mb
Tc = lam + size/b

plt.scatter(range(2,len(times)+2),times_float)
plt.plot([25,48],[25*(2*Tc),48*(2*Tc)])
plt.grid(True)
plt.locator_params(axis='y', nbins=9)
plt.savefig('time_plot-48.png')
plt.close()


