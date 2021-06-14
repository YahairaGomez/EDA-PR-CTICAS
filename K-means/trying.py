import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

# Before clustering
df = pd.read_csv("data_set1.csv")
my_plot=df.plot("Start_Lon", "Start_Lat", kind="scatter")

plt.title("Original dataset ")
plt.show()

# After clustering
plt.figure()
df = pd.read_csv("output.csv")

sns.scatterplot(x=df.x, y=df.y,  data=df, hue=df.c, palette="Set1", alpha=1, s=80)

plt.xlabel("Longitude")
plt.ylabel("Latitude")
plt.title("Latitude based on longitude")

plt.show()
