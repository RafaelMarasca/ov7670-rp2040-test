import cv2
import numpy as np
import serial

HEIGHT = 144
WIDTH = 176

while 1:
    list =[]

    with serial.Serial('COM14', 115200) as ser:
        ser.reset_input_buffer()
        while(len(list)<176*144*2):
            list =ser.read(size = 176*144*2)

    y = []
    cb = []
    cr = []


    for index,element in enumerate(list):
        if (index)%2 != 0:
            y.append(element)
        else:
            if len(cb) == len(cr):
                cb.append(element)
                cb.append(element)
            else:
                cr.append(element)
                cr.append(element)

    y = np.array(y,dtype = np.uint8)
    cb = np.array(cb,dtype = np.uint8)
    cr = np.array(cr,dtype = np.uint8)

    y = np.uint8(y.reshape((144,176), order = 'C'))
    cb = cb.reshape((144,176), order = 'C')
    cr = cr.reshape((144,176), order = 'C')


    r = 1.164*(y) + (1.793* (cr)) -248.1
    g = 1.164*(y) - (0.2132*(cb)) - (0.5329*(cr)) +76.878
    b = 1.164*(y) + (2.112*(cb)) -289.02

    r = np.clip(r,0,255)
    g = np.clip(g,0,255)
    b = np.clip(b,0,255)

    r = np.uint8(np.array(r).reshape((HEIGHT,WIDTH)))
    g = np.uint8(np.array(g).reshape((HEIGHT,WIDTH)))
    b = np.uint8(np.array(b).reshape((HEIGHT,WIDTH)))


    im = cv2.merge([b.transpose(),g.transpose(),r.transpose()])
    #im = cv2.merge([y,y,y])
    cv2.namedWindow('dysplay', cv2.WINDOW_NORMAL)
    cv2.resizeWindow('dysplay', 500,500)
    cv2.imshow('dysplay', im)
    cv2.waitKey()