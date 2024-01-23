import numpy as np
import matplotlib.pyplot as plt

tof_matrix_size=8,8

fig, ax = plt.subplots()

min_val, max_val = 0, 15

intersection_matrix = np.random.randint(0, 10, size=tof_matrix_size)

ax.matshow(intersection_matrix, cmap=plt.cm.Blues)

for i in range(tof_matrix_size[0]):
    for j in range(tof_matrix_size[1]):
        c = intersection_matrix[j,i]
        ax.text(i, j, str(c), va='center', ha='center')

plt.show()
