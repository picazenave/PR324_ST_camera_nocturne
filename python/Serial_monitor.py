import numpy as np
import cv2
import serial
import time
import matplotlib.pyplot as plt
#=======================================
jpg_header= [0x11,0x12,0x55,0x44,0xFF]

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
#=======================================
def is_jpg_header(s):
    for i in range(len(jpg_header)):
        if(jpg_header[i]!=s[i]):
            return False
    return True

#=======================================
window_name='Tof matrix'

ser = serial.Serial('COM7', 1000000)
print(ser.name)
ser.reset_input_buffer()
ser.reset_output_buffer()

cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)

while(True):

    s=ser.read_all()
    size_s=len(s)
    if(size_s>0):
        print(bcolors.OKGREEN + "[PYTHON]"+bcolors.ENDC+"size_s="+str(size_s))
    i=0
    while i<size_s:#try to find jpg_header
        if(i+5<size_s):
            if(is_jpg_header(s[i:i+5])):
                print(bcolors.OKCYAN + "[PYTHON]"+bcolors.ENDC+"header=0x"+' 0x'.join(format(x, '02x') for x in s[i:i+5]))
                i=i+5
                print(bcolors.OKCYAN + "[PYTHON]"+bcolors.ENDC+"après header+jpg=0x"+' 0x'.join(format(x, '02x') for x in s[i:i+5]))
                im_len=int.from_bytes([s[i],s[i+1]],"big")
                i=i+2
                print(bcolors.OKGREEN + "[PYTHON]"+bcolors.ENDC+"im_len="+str(im_len)+"|size_s="+str(size_s)+"|i="+str(i))

                #receive JPG
                bytes_received=size_s-i #part of jpeg already received
                to_be_received=im_len-bytes_received
                print(bcolors.OKGREEN + "[PYTHON]"+bcolors.ENDC+"to_be_received="+str(to_be_received)+"|in waiting="+str(ser.in_waiting))
                s_array: bytearray
                s_array=[]
                s_array.extend(s[i:])#add received part
                s=ser.read(to_be_received-1)
                s_array.extend(s)
                s_byte=np.frombuffer(bytes(s_array),dtype=np.uint8)

                try :
                    im = cv2.imdecode(s_byte,cv2.IMREAD_UNCHANGED)
                    cv2.imshow(window_name, im)
                    cv2.waitKey(1) 
                except :
                    print(bcolors.FAIL + "[PYTHON]"+bcolors.ENDC+"cv2 decode error")
                i=i+im_len
        if i<size_s :
            print(chr(s[i]), end='')
        i=i+1
        



    
# packet = bytearray()
# packet.append(0x41)
# packet.append(0x42)
# packet.append(0x43)

# ser.write(packet)
