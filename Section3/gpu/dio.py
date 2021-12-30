import pandas as pd

df = pd.read_fwf('Grafici/Two-nodes-48-cores.txt', header=None)
print(df)
df.to_csv('Grafici/Two-nodes-48-cores.csv', index = None)
