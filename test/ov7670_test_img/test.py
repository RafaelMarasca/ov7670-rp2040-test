import cv2
import numpy as np
import serial

HEIGHT = 144
WIDTH = 176

while 1:
    list =[]

    with serial.Serial('COM14', 115200) as ser:
        ser.reset_input_buffer()
        while(len(list)<176*144):
            list =ser.read(size = 176*144)

    y = []

    for index,element in enumerate(list):
        y.append(element)

    y = np.array(y,dtype = np.uint8)

    y = np.uint8(y.reshape((144,176), order = 'C'))

    im = cv2.merge([y,y,y])
    cv2.namedWindow('dysplay', cv2.WINDOW_NORMAL)
    cv2.resizeWindow('dysplay', 500,500)
    cv2.imshow('dysplay', im)
    cv2.waitKey()