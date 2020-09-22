import numpy as np
import cv2
from matplotlib import pyplot as plt

"""
a,b = np.loadtxt('BubbleSort/valores_C++.txt',delimiter = ',',unpack = True)
c,d = np.loadtxt('CountingSort/valores_C++.txt',delimiter = ',',unpack = True)
e,f = np.loadtxt('HeapSort/valores_C++.txt',delimiter = ',',unpack = True)
g,h = np.loadtxt('InsertionSort/valores_C++.txt',delimiter = ',',unpack = True)
i,j = np.loadtxt('MergeSort/valores_C++.txt',delimiter = ',',unpack = True)
k,l = np.loadtxt('Quicksort/valores_C++.txt',delimiter = ',',unpack = True)
m,n = np.loadtxt('SelectionSort/valores_C++.txt',delimiter = ',',unpack = True)
"""
a,b,c = np.loadtxt('salida.out',delimiter = ',',unpack = True)

plt.plot(a,c, color = "green",label = "Comun")
plt.plot(a,b, color = "blue",label = "Bloques")
plt.title('Comparación de Recorrido de Matrices')
plt.xlabel('Tamaño de matriz')
plt.ylabel('Tiempo')
plt.legend()
plt.show();
