# calc time it takes to trasnmit tof matrix over uart

uart_speed=115200
frame_size=10
tof_matrix_size=8*8

time_one_frame=frame_size/uart_speed
print('time one frame : ' + str(time_one_frame) + 's')
time_tof_matrix=tof_matrix_size*time_one_frame
print('time tof matrix : ' +str(time_tof_matrix) + 's')
refresh_rate=1/time_tof_matrix
print('refresh rate : ' +str(refresh_rate) + 'Hz')

refresh_rate=1/0.025
print('refresh rate calc : ' +str(refresh_rate) + 'Hz')