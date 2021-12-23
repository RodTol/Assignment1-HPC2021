import matplotlib.pyplot as plt

files_to_be_opened = ['thin_2_nodes_4_processes.txt', 'thin_2_nodes_8_processes.txt', 'thin_2_nodes_12_processes.txt', 'thin_2_nodes_24_processes.txt', 'thin_2_nodes_48_processes.txt',
'thin_2_sockets_4_processes.txt', 'thin_2_sockets_8_processes.txt', 'thin_2_sockets_12_processes.txt', 'thin_same_socket_4_processes.txt', 'thin_same_socket_8_processes.txt', 
'thin_same_socket_12_processes.txt'] 
#'gpu_2_nodes_4_processes.txt', 'gpu_2_nodes_8_processes.txt', 'gpu_2_nodes_12_processes.txt', 'gpu_2_nodes_24_processes.txt', 
#'gpu_2_nodes_48_processes.txt', 'gpu_2_sockets_4_processes.txt', 'gpu_2_sockets_8_processes.txt', 'gpu_2_sockets_12_processes.txt', 'gpu_same_socket_4_processes.txt', 
#'gpu_same_socket_4_processes.txt', 'gpu_same_socket_8_processes.txt', 'gpu_same_socket_12_processes.txt']

# MANCANO I SERIAL

P =[]

for el in files_to_be_opened :

    filename = el
    file = open(filename, 'r')
    file = file.readlines()

    if ((filename.split(sep='_')[0] == 'thin') and filename.split(sep='_')[2] == 'nodes' ) :
        lambd = 0.98
        bandwidth = 12160.37
    elif ((filename.split(sep='_')[0] == 'thin') and filename.split(sep='_')[2] == 'sockets' ) :
        lambd = 0.39
        bandwidth = 5596.40
    elif ((filename.split(sep='_')[0] == 'thin') and filename.split(sep='_')[2] == 'socket' ) :
        lambd = 0.19
        bandwidth = 6443.60
    
    elif ((filename.split(sep='_')[0] == 'gpu') and filename.split(sep='_')[2] == 'nodes' ) :
        lambd = 1.34
        bandwidth = 12174.96
    elif ((filename.split(sep='_')[0] == 'gpu') and filename.split(sep='_')[2] == 'sockets' ) :
        lambd = 0.42
        bandwidth = 4743.65
    elif ((filename.split(sep='_')[0] == 'gpu') and filename.split(sep='_')[2] == 'socket' ) :
        lambd = 0.22
        bandwidth = 6151.94
    else :
        raise ValueError


    Nx = int(file[5].split()[4])
    Ny = int(file[5].split()[5])
    Nz = int(file[5].split()[6])

    _k = 0
    process_grid = [Nx, Ny, Nz]
    for num in process_grid :
        if (num > 1) :
            _k += 1
    
    
    # analysis of jacobi solver


    process_grid_sorted = sorted(process_grid)

    N = Nx*Ny*Nz
    Ts = 22.0850230450   # TO BE FIXED

    L = 1200

    B = bandwidth 
    Tl = lambd * (10**(-6))  # in secondi 
    k = _k*2
    c = (L**2)*k*2*8 *10**(-6)  #(L/process_grid_sorted[0])*(L/process_grid_sorted[1])
    Tc = (c/B)+(k*Tl)

    P_1 = ((L**3)/Ts) *10**(-6)
    P.append( ((L**3)*N)/(Ts + Tc) *10**(-6) )

    P_local = ((L**3)*N)/(Ts + Tc) *10**(-6) 
    slodown = (P_1*N)/P_local

    print(el, 'p_local', P_local, 'slodown', slodown)
    #print(el, P, speedup)
    print('c: ', c)
    print('Tc :', Tc)
    #print('T1/P', Ts/P)
    #print(P_1)


plt.plot([4,8,12,24,48], P[0:5])
plt.scatter([4,8,12,24,48], P[0:5])
plt.plot([1, 48], [P_1, (((1200**3)*48)/Ts )*10**(-6)])
plt.savefig('fig.png')





