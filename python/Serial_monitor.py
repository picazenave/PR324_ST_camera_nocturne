import numpy as np
import cv2
import serial
import time
import matplotlib.pyplot as plt
#=======================================
jpg_header= [0x11,0x12,0x55,0x44,0xFF]
tof_header= [0xAA,0x55,0x32,0x01,0x21]

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

def is_tof_header(s):
    for i in range(len(tof_header)):
        if(tof_header[i]!=s[i]):
            return False
    return True

#=======================================
window_name='Tof matrix'


im_tof = np.zeros((1600,1600,3), np.uint8)
im_camera = np.zeros((1600,1600,3), np.uint8)

ser = serial.Serial('COM7', 2000000)
print(ser.name)
ser.reset_input_buffer()
ser.reset_output_buffer()

cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)

while(True):

    s=ser.read_all()
    size_s=len(s)
    if(size_s>0):
        pass#print(bcolors.OKBLUE + "[PYTHON]"+bcolors.ENDC+"size_s="+str(size_s))
    i=0
    while i<len(s):#try to find jpg_header
        if(i+5<len(s)):
            if(is_jpg_header(s[i:i+5])):
                #print(bcolors.OKCYAN + "[PYTHON]"+bcolors.ENDC+"header_jpeg=0x"+' 0x'.join(format(x, '02x') for x in s[i:i+5]))
                i=i+5
                #print(bcolors.OKCYAN + "[PYTHON]"+bcolors.ENDC+"après header+jpg=0x"+' 0x'.join(format(x, '02x') for x in s[i:i+5]))
                im_len=int.from_bytes([s[i],s[i+1]],"big")
                i=i+2
                #print(bcolors.OKGREEN + "[PYTHON]"+bcolors.ENDC+"im_len="+str(im_len)+"|size_s="+str(size_s)+"|i="+str(i))

                #receive JPG
                bytes_received=size_s-i #part of jpeg already received
                to_be_received=im_len-bytes_received
                #print(bcolors.OKGREEN + "[PYTHON]"+bcolors.ENDC+"to_be_received="+str(to_be_received)+"|in waiting="+str(ser.in_waiting))
                s_array: bytearray
                s_array=[]
                s_array.extend(s[i:])#add received part
                s=ser.read(to_be_received-1)
                s_array.extend(s)
                s_byte=np.frombuffer(bytes(s_array),dtype=np.uint8)

                try :
                    im = cv2.imdecode(s_byte,cv2.IMREAD_UNCHANGED)
                    resized = cv2.resize(im, (1600,1600), interpolation = cv2.INTER_AREA)
                    im_camera=resized
                    im_camera=cv2.rotate(im_camera,cv2.ROTATE_90_COUNTERCLOCKWISE)
                    im_stack=cv2.hconcat([im_camera,im_tof]) 
                    cv2.imshow(window_name, im_stack)
                    cv2.waitKey(1) 
                    #print(bcolors.OKGREEN + "[PYTHON]"+bcolors.ENDC+"cv2 decode OK")
                except :
                    print(bcolors.FAIL + "[PYTHON]"+bcolors.ENDC+"cv2 decode error")
                i=i+im_len
            elif(is_tof_header(s[i:i+5])):
                #print(bcolors.OKCYAN + "[PYTHON]"+bcolors.ENDC+"header_tof=0x"+' 0x'.join(format(x, '02x') for x in s[i:i+5]))
                i=i+5
                #print(bcolors.OKCYAN + "[PYTHON]"+bcolors.ENDC+"après header+tof=0x"+' 0x'.join(format(x, '02x') for x in s[i:i+5]))
                #receive tof data
                bytes_received=size_s-i #part of tof already received
                to_be_received=64*3+3-bytes_received
                #print(bcolors.OKGREEN + "[PYTHON]"+bcolors.ENDC+"to_be_received="+str(to_be_received)+"|in waiting="+str(ser.in_waiting))
                s_array: bytearray
                s_array=[]
                s_array.extend(s[i:])#add received part
                s=ser.read(to_be_received)
                s_array.extend(s)
                s_byte=np.frombuffer(bytes(s_array),dtype=np.uint8)

                tof_array=s_byte[:64]
                tof_status=s_byte[64:64*2]
                tof_target=s_byte[64*2:64*3]
                tof_target_x=s_byte[64*3]
                tof_target_y=s_byte[64*3+1]
                tof_target_is_tracking=s_byte[64*3+2]
                #convert to image
                tof_matrix=np.matrix(tof_array.reshape(8,8))
                #img is 8x8 so upscale
                scale_percent = 200*100 # percent of original size
                width = int(tof_matrix.shape[1] * scale_percent / 100)
                height = int(tof_matrix.shape[0] * scale_percent / 100)
                dim = (width, height)
                # resize image
                resized = cv2.resize(tof_matrix, dim, interpolation = cv2.INTER_AREA)
                resized = cv2.cvtColor(resized,cv2.COLOR_GRAY2RGB)
                #resized = cv2.flip(resized,1)
                #print('Resized Dimensions : ',resized.shape)
                square_size=resized.shape[0]/8 #size of each square
                #then draw numbers
                for i in range(64):
                    font                   = cv2.FONT_HERSHEY_SIMPLEX
                    position               = (int(square_size*i-int(i/8)*square_size*8)+int(square_size/2),int(int(i/8)*square_size+square_size/2)-20)
                    fontScale              = 1
                    fontColor              = (255,0,0)#BGR
                    thickness              = 2
                    lineType               = 2
                    cv2.putText(resized,str(i),position,font,fontScale,fontColor,thickness,lineType)
                    fontColor              = (200,200,200)
                    position               = (int(square_size*i-int(i/8)*square_size*8)+int(square_size/2),int(int(i/8)*square_size+square_size/2)+20)
                    cv2.putText(resized,str(tof_array[i]*10), position,font,fontScale,fontColor,thickness,lineType)
                    im_tof=resized
                
                if(tof_target_is_tracking):#BGR
                    p2= (int((tof_target_x)*square_size)+int(square_size/2),int(tof_target_y*square_size+square_size/2)-20)
                    i=4+4*8#center index
                    p_center= (int(square_size*i-int(i/8)*square_size*8)+int(square_size/2),int(int(i/8)*square_size+square_size/2)-20)
                    cv2.arrowedLine(resized,p_center,p2,(0,0,255),thickness=8, line_type=8, shift=0)
                    cv2.circle(resized,p2, 65, (0,0,255), thickness=8, lineType=8, shift=0)
                    font                   = cv2.FONT_HERSHEY_SIMPLEX
                    position               = (10,80)
                    fontScale              = 3
                    fontColor              = (0,0,255)#BGR
                    thickness              = 3
                    lineType               = 2

                    cv2.putText(resized,"is_tracking",position,font,fontScale,fontColor,thickness,lineType)
                    position               = (10,120)
                    fontScale              = 1
                    
                try :
                    #im_stack=cv2.hconcat([im_camera,im_tof]) 
                    cv2.imshow(window_name, im_tof)
                    cv2.waitKey(1)
                    
                    #print(bcolors.OKGREEN + "[PYTHON]"+bcolors.ENDC+"cv2 imshow OK")
                except :
                    print(bcolors.FAIL + "[PYTHON]"+bcolors.ENDC+"cv2 imshow error")

                i=i+64*3+3

        if i<len(s) :
            print(chr(s[i]), end='')
        i=i+1
        



    
# packet = bytearray()
# packet.append(0x41)
# packet.append(0x42)
# packet.append(0x43)

# ser.write(packet)
