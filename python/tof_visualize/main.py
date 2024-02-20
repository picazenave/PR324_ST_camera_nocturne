import numpy as np
import cv2
import math
import serial
import time
import matplotlib.pyplot as plt
from numpy import genfromtxt
import signal
import sys

from enum import Enum
class Season(Enum):
    SPRING = 1
    SUMMER = 2
    AUTUMN = 3
    WINTER = 4

my_data = genfromtxt('C:/Users/pierr/Documents/GitHub/PR324_ST_camera_nocturne/python/tof_visualize/putty.log', delimiter=',')
my_horodatage=my_data[:,:1]
my_nbtarget=my_data[:,65:129]
my_status=my_data[:,129:]
my_data=my_data[:,1:65]
my_data_original=my_data
my_data=(my_data/(max(my_data[0])))*255


#make include file for pre-calculated values
valid_status=[5,9]
valid_status = set(valid_status)
computed_data=np.empty_like(my_data_original)
for counter in range(len(my_data_original)):
    for i in range(64):
        #if(my_status[counter][i]==5 or my_status[counter][i]==6 or my_status[counter][i]==8 or my_status[counter][i]==9 or my_status[counter][i]==10 or my_status[counter][i]==12 or my_status[counter][i]==13 or my_status[counter][i]==13):
        if(my_status[counter][i] in valid_status and my_nbtarget[counter][i]>0):
            computed_data[counter][i]=my_data_original[counter][i]
        else : #status bit set so problem
            computed_data[counter][i]=2550

x_gen=range(8)
y_gen=range(8)
center_position_x=4
center_position_y=4

with open('temp/auto_gen.h', 'w+') as outfile:
    outfile.write("#ifndef auto_gen_h\n#define auto_gen_h\n\n")
    outfile.write("#include \"main.h\"\n\n")
    #write pre computed distances
    outfile.write("float distance_matrix[64] = {")
    for x in x_gen:
        outfile.write("\n")
        for y in y_gen:
            #x=x-(int(x/8)*8)
            #y=int(y/8)
            dx = (center_position_x-x)
            dy = (center_position_y-y)
            distance_to_center = math.sqrt(dx*dx + dy*dy)
            outfile.write("{:.5f},".format(distance_to_center))
    outfile.write("};\n")
    
    #write raw TOF data
    outfile.write("uint16_t tof_data[{size}]".format(size=(len(computed_data)*64))+" = {")
    for i in range(len(computed_data)):
        outfile.write("\n")
        for j in range(64):
            outfile.write("{value},".format(value=int(computed_data[i,j])))
    outfile.write("};\n")

    

    outfile.write("\n#endif\n")


    


print("mean_fps="+str(1000/np.mean(np.abs(my_horodatage[:-1] - my_horodatage[1:]))))

window_name='Tof matrix'

# ser = serial.Serial('COM7', 2000000)
# print(ser.name)
# ser.reset_input_buffer()
# ser.reset_output_buffer()


cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)
cv2.resizeWindow(window_name, 800 , 800)

tof_matrix_size=8,8

counter=0
end_counter=len(my_data)
time_loop=np.empty(end_counter)

#init background
background=np.empty(64,dtype=np.int16)
for i in range(64):
    if(my_status[counter][i] in valid_status and my_nbtarget[counter][i]>0):
            background[i]=my_data_original[counter][i]
    else:
        background[i]=2550
#init vars
is_tracking=False
should_capture=True
local_max=0
local_max_index=0
while(True):
    start = time.time()
    # ser.write(b'\x55')
    my_array = np.empty(64,dtype=np.uint8)
    my_array_original = np.empty(64,dtype=np.int16)
    # s = ser.read(8*8)
    #ser.reset_input_buffer() # loose synchro
    
    for i in range(64):
        my_array_original[i]=my_data_original[counter][i]
        #if(my_status[counter][i]==5 or my_status[counter][i]==6 or my_status[counter][i]==8 or my_status[counter][i]==9 or my_status[counter][i]==10 or my_status[counter][i]==12 or my_status[counter][i]==13 or my_status[counter][i]==13):
        if(my_status[counter][i] in valid_status and my_nbtarget[counter][i]>0):
            my_array[i]=my_data[counter][i]
        else : #status bit set so problem
            my_array[i]=255
            my_array_original[i]=2550

#==================================================
# compute
#==================================================  
#find closest pixel and exclude pixel too close to background
    possible_target_index=[]
    treshold=100
    local_max=0
    local_max_index=0
    local_min=2550
    local_min_i=0
    distance_to_center=0
    is_tracking = False
    #find all i that could be target
    for i in range(64):
        temp=abs(my_array_original[i]-background[i])
        if temp>treshold and my_status[counter][i] in valid_status and my_nbtarget[counter][i]>0:
            possible_target_index.append(i)
            is_tracking=True              

    if(is_tracking):
        #now that we have a list of possible target index, compute closest to camera
        # for i in range(64):
        #     if possible_target_index[i]==0:#at the end of list, should be -1 in C
        #         break
        #     if(my_array_original[possible_target_index[i]]<local_min):
        #         local_min_i=possible_target_index[i]
        #         local_min=my_array_original[possible_target_index[i]]

        #better : list of possible target index, compute center of mass
        x_list=np.array(possible_target_index)
        x_list=x_list-(np.array(x_list/8,dtype=np.int16)*8)
        y_list=np.array(possible_target_index)
        y_list=np.array(y_list/8,dtype=np.int16)

        x=round(np.mean(x_list))
        y=round(np.mean(y_list))
        dx = (center_position_x-x)
        dy = (center_position_y-y)
        distance_to_center = math.sqrt(dx*dx + dy*dy)

        #angleInDegrees_center_to_p = math.atan2(dy, dx) * 180 / 3.141
#==================================================
# Image part
#==================================================            
    #convert to image
    tof_matrix=np.matrix(my_array.reshape(8,8))
    #img is 8x8 so upscale
    scale_percent = 200*100 # percent of original size
    width = int(tof_matrix.shape[1] * scale_percent / 100)
    height = int(tof_matrix.shape[0] * scale_percent / 100)
    dim = (width, height)
    # resize image
    resized = cv2.resize(tof_matrix, dim, interpolation = cv2.INTER_AREA)
    resized = cv2.cvtColor(resized,cv2.COLOR_GRAY2RGB)
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
        cv2.putText(resized,str(my_array_original[i]), position,font,fontScale,fontColor,thickness,lineType)

#==================================================
# draw debug infos
#==================================================  
    if(is_tracking):#BGR
        #draw all possible target index
        for i in range(len(possible_target_index)):
            i=possible_target_index[i]
            p= (int(square_size*i-int(i/8)*square_size*8)+int(square_size/2),int(int(i/8)*square_size+square_size/2)-20)
            cv2.circle(resized,p, 50, (255,128,255), thickness=6, lineType=8, shift=0)
        
        i=local_min_i
        p2= (int(x*square_size)+int(square_size/2),int(y*square_size+square_size/2)-20)
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
        cv2.putText(resized,"distance center="+"{:.2f}".format(distance_to_center),position,font,fontScale,fontColor,thickness,lineType)
        if(should_capture):
            fontScale              = 3
            position               = (10,200)
            fontColor              = (255,0,0)
            cv2.putText(resized,"should_capture",position,font,fontScale,fontColor,thickness,lineType)


    cv2.imshow(window_name, resized)
    cv2.waitKey(1)  # it's needed, but no problem, it won't pause/wait
    time.sleep(1/15-(time.time()-start))#to maintain 15fps
    time_loop[counter]=time.time()-start
    counter=counter+1
    if counter == end_counter:
        break

#imstack = np.hstack((imstack,im)) pour 2 en horizontal

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
