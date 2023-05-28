import numpy as np
import cv2
import time
import os
import matplotlib.pyplot as plt

cam = cv2.VideoCapture(0)

cv2.namedWindow("test")

img_counter = 0

while True:
    ret, frame = cam.read()
    if ret:
        break
    else:
        print("failed to grab frame")

k = cv2.waitKey(1)
img_name = "img.png"
cv2.imwrite(os.path.join("/home/raspi/robot",img_name), frame)
print("{} written!".format(img_name))
print(cv2.imread(os.path.join("/home/raspi/robot",img_name)).shape)


cam.release()
cv2.destroyAllWindows()







