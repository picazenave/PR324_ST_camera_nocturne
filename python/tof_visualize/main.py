import numpy as np
import cv2
import serial
import time
import matplotlib.pyplot as plt

window_name='Tof matrix'

ser = serial.Serial('COM6', 115200)
print(ser.name)


cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)

tof_matrix_size=8,8

time_loop=np.empty(1024*1)
counter=0
end_counter=1024*1-1
while(True):
    start = time.time()
    ser.write(b'\x55')
    my_array = np.empty(64,dtype=np.uint8)
    s = ser.read_until("\r\n",8*8+2)[:8*8]
    
    for i in range(64):
        my_array[i]=s[i]
    tof_matrix=np.matrix(my_array.reshape(8,8))

    cv2.imshow(window_name, tof_matrix)
    cv2.waitKey(1)  # it's needed, but no problem, it won't pause/wait

    time_loop[counter]=time.time()-start
    counter=counter+1
    if counter == end_counter -1:
        break

print('Mean FPS :'+ str(1/(sum(time_loop)/end_counter)))
plt.plot(time_loop)
plt.show()


