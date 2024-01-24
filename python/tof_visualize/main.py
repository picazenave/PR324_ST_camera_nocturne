import numpy as np
import cv2
import serial
import time
import matplotlib.pyplot as plt

window_name='Tof matrix'

ser = serial.Serial('COM7', 1000000)
print(ser.name)
ser.reset_input_buffer()
ser.reset_output_buffer()


cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)

tof_matrix_size=8,8

counter=0
end_counter=1024*1
time_loop=np.empty(end_counter)

while(True):
    start = time.time()
    ser.write(b'\x55')
    my_array = np.empty(64,dtype=np.uint8)
    s = ser.read(8*8)
    #ser.reset_input_buffer() # loose synchro
    
    for i in range(64):
        my_array[i]=s[i]
    tof_matrix=np.matrix(my_array.reshape(8,8))

    cv2.imshow(window_name, tof_matrix)
    cv2.waitKey(1)  # it's needed, but no problem, it won't pause/wait

    time_loop[counter]=time.time()-start
    counter=counter+1
    if counter == end_counter:
        break

print('Mean FPS :'+ str(1./(sum(time_loop)/end_counter)))
print('Min FPS :'+ str(1./max(time_loop)))
print('Max FPS :'+ str(1./min(time_loop[time_loop != 0])))
plt.plot(time_loop)
plt.show()

# packet = bytearray()
# packet.append(0x41)
# packet.append(0x42)
# packet.append(0x43)

# ser.write(packet)
