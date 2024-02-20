import numpy as np
import cv2
import serial
import time
import matplotlib.pyplot as plt
from enum import Enum


window_name='Serial ESPCAM'

ser = serial.Serial('COM9', 2000000,timeout=4)
print(ser.name)
ser.reset_input_buffer()
ser.reset_output_buffer()


cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)

counter=0
end_counter=16*2
time_loop=np.empty(end_counter)
len_loop=np.empty(end_counter)

ser.reset_input_buffer()
ser.read_all()
# Config sequence

#check if already configured
ser.write(b'\xAA')
a=0
a=ser.read(1)
if a==b'\x04' :
    print("ACK setup already done:"+str((a).hex()))
else :
    #else configure
    ser.write(b'\xA1')
    time.sleep(3)
    ser.write(b'\xAA')
    a=0
    a=ser.read(1)
    if a!=b'\x00' :
        print("ACK setup not done KO:"+str((a).hex()))
        ser.read_all()
        a=ser.read(1)
        retry_counter=0
        while a!=b'\x00' and retry_counter<10:
            retry_counter=retry_counter+1
            a=ser.read(1)
            print("ACK setup not done KO:"+str((a).hex())+"  retry:"+str(retry_counter))
        if(a!=b'\x00'):  
            exit()

    ser.write(b'\xA1')
    #brightness
    ser.write(b'\x34')
    ser.write(b'\x00')
    #special effect
    ser.write(b'\x33')
    ser.write(b'\x00')
    #jpg quality
    ser.write(b'\x22')
    ser.write(b'\x1E')
    #frame size
    ser.write(b'\x11')
    ser.write(b'\x06')

    #end config
    ser.write(b'\xA2')

    ser.write(b'\xAA')
    a=0
    a=ser.read(1)
    if a!=b'\x04' :
        print("ACK setup done KO:"+str((a).hex()))
        exit()

print("INIT DONE")
#// 800x600
s_array: bytearray
s_array=[]
while(True):
    start = time.time()

    ser.write(b'\x55')
    s=ser.read(2)
    im_len=int.from_bytes(s, "big")
    print('im_len :'+ str(im_len))
    # ser.write(b'\x55')
    # s = ser.read(im_len)
    if im_len == 65000:
        while im_len==65000 :
            ser.write(b'\x55')
            s = ser.read(im_len)
            s_array.extend(s)
            ser.write(b'\x55')
            s=ser.read(2)
            im_len=int.from_bytes(s, "big")
            print('im_len boucle :'+ str(im_len))

        print("derniere lecture après boucle")
        ser.write(b'\x55')
        s = ser.read(im_len)
        s_array.extend(s)
        s_byte=np.frombuffer(bytes(s_array),dtype=np.uint8)
        print('s_array apres boucle size :'+ str(len(s_array)))
    else :
        print("lecture sans boucle")
        ser.write(b'\x55')
        s = ser.read(im_len)
        s_byte=np.frombuffer(s,dtype=np.uint8)


    #s_byte=np.frombuffer(s,dtype=np.uint8)
    
        
    #ser.reset_input_buffer() # loose synchro
    try :
        im = cv2.imdecode(s_byte,cv2.IMREAD_UNCHANGED)
        cv2.imshow(window_name, im)
        cv2.waitKey(1) 
    except :
        print("cv2 decode error")
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        ser.read_all()
    
    time_loop[counter]=time.time()-start
    len_loop[counter]=im_len
    counter=counter+1
    if counter == end_counter:
        break

print('Mean FPS :'+ str(1./(sum(time_loop)/end_counter)))
print('Min FPS :'+ str(1./max(time_loop)))
print('Max FPS :'+ str(1./min(time_loop[len_loop != 0])))

print('Mean len :'+ str((sum(len_loop)/end_counter)))
print('Min len :'+ str(max(len_loop)))
print('Max len :'+ str(min(len_loop)))

print('im shape :'+ str(im.shape))
im

plt.subplot(2,1,1)
plt.plot(time_loop)
plt.subplot(2,1,2)
plt.plot(len_loop)
plt.show()

# packet = bytearray()
# packet.append(0x41)
# packet.append(0x42)
# packet.append(0x43)

# ser.write(packet)
