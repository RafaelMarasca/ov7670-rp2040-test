import cv2
import numpy as np
import serial

HEIGHT = 144
WIDTH = 176


# img = cv2.imread("shrek.jpg")
# img_yuv = cv2.cvtColor(img, cv2.COLOR_BGR2YUV)
# y, u, v = cv2.split(img_yuv)


# y = (np.uint8(y.reshape(240*240))).tolist()
# u = (np.uint8(u.reshape(240*240))).tolist()
# v = (np.uint8(v.reshape(240*240))).tolist()

# with open('shrek.txt', 'w') as f:
#     f.write('char shrek [] = {')
#     for (i,j,k) in zip(y,u,v):
#         f.write(str(i))
#         f.write(',')
#         # list.append(j)
#         # list.append(i)
#         # list.append(k)
#         # list.append(i)
#     f.write('};')

list =[]

with serial.Serial('COM14', 115200) as ser:
    ser.reset_input_buffer()
    while(len(list)<176*144*3):
        list =ser.read(size = 176*144*3)

r = []
g = []
b = []

for index, element in enumerate(list):
    if len(r) == len(g):
        if len(b) == len(g):
            r.append(element)
        else:
            b.append(element)
    else:
        g.append(element)
        
            
# y_new = []
# cb = []
# cr = []

# for index,element in enumerate(list):
#     if (index)%2 != 0:
#         y_new.append(element)
#     # else:
#     #     if len(cb) == len(cr):
#     #         cr.append(element)
#     #         cr.append(element)
#     #     else:
#     #         cb.append(element)
#     #         cb.append(element)


# y_new = np.clip(y_new, 0,255);

# # y_new = np.array(y_new,dtype = np.uint8)
# # cb = np.array(cb,dtype = np.uint8)
# # cr = np.array(cr,dtype = np.uint8)

# y_new = np.uint8(y_new.reshape((144,176), order = 'C'))
# # cb = cb.reshape((144,176), order = 'C')
# cr = cr.reshape((144,176), order = 'C')



# r = y_new + (1.403* (cr - 128))
# g = y_new - (0.344*(cb - 128)) - (0.714*(cr-128))
# b = y_new + (1.773*(cb-128))

# im = cv2.merge([y_new,cb,cr])
# im = cv2.cvtColor(im, cv2.COLOR_YUV2BGR)
# cv2.namedWindow('dysplay', cv2.WINDOW_NORMAL)
# cv2.resizeWindow('dysplay', 500,500)
# cv2.imshow('dysplay', im)
# cv2.waitKey()


# r = 1.164*(y_new) + (1.793* (cr)) -248.1
# g = 1.164*(y_new) - (0.2132*(cb)) - (0.5329*(cr)) +76.878
# b = 1.164*(y_new) + (2.112*(cb)) -289.02

# r = np.clip(r,0,255)
# g = np.clip(g,0,255)
# b = np.clip(b,0,255)

r = np.uint8(np.array(r).reshape((HEIGHT,WIDTH)))
g = np.uint8(np.array(g).reshape((HEIGHT,WIDTH)))
b = np.uint8(np.array(b).reshape((HEIGHT,WIDTH)))


im = cv2.merge([b,g,r])
#im = cv2.merge([y_new,y_new,y_new])
cv2.namedWindow('dysplay', cv2.WINDOW_NORMAL)
cv2.resizeWindow('dysplay', 500,500)
cv2.imshow('dysplay', im)
cv2.waitKey()