lines = []

for i in range(2,4):
    f = open('out.{}'.format(i), 'r')
    lines = f.readlines()
    f.close

    for line in lines:
        if 'The mean time is' in line:
            for i in line:
                
            print(line)
