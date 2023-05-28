# Autonomous robotic fire detection using Deep Learning YOLOv5 algorithm and Rasberry pi 4

As the name suggests, this project includes two different parts:<br>
1. Creating a Fire Detection model, on Raspberry pi 4 (2GB RAM), using **custom dataset and Transfer Learning with YOLOv5 algorithm**.
2. Autonomous robot which drives to the target once it is detected a flame.<br>


## Custom Dataset

In this project, our mission is to detect flames of fire in an indoor space.
Therefore, We assembled a unique dataset which contains flames of candles, in different scenarios and lightning conditions, in a specific room.

This dataset contains 630 images with a size of 640x640.<br>

The annotation process was executed with Roboflow, so in the end of the process, we had a custom yaml file in YOLOv5 format, which helps the model to know how to decide whether a flame is detected or not.<br>
With Roboflow, we managed to implement various augmentations upon the dataset, so that it is vary diversified.
Therefore, the model will be more resilient and capable of dealing with frames with high brightness, blur, different rotations and angles, concealments and noise.  

A summarization of the custom dataset generation:<br>

![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/9f9265f0-eddb-4265-bdca-b0fdf124e777)

## Setup

1. Clone YOLOv5 base code and install requirements on the Raspberry pi:<br>
![image](https://user-images.githubusercontent.com/121958931/220930034-1bab3649-2123-40ce-bb31-117c3a29f37b.png)

2. Import the desired packages and connect to ROBOFLOW:<br>
![image](https://user-images.githubusercontent.com/121958931/220946259-a0781378-2e15-425e-a6b9-efd684b4fdde.png)

3. Train our custom YOLOv5 model:<br>
Important: **we trained the model on a strong computer with Google's GPU, and not on Raspberry pi due to his limited resources**<br>
![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/6bfd1038-81f2-4f1e-9d89-7d92adc1dcb8)

The training process:<br>
![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/9f56abd9-2401-44c2-a1c6-af85f3352a87)

4. Run model with trained weights (**on Raspberry pi**):<br>
![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/89351613-3a21-470f-83b5-061b9720e174)

## Results

![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/ad0ca6f7-3500-4de2-a681-ff7f21e2fed9)<br>
![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/a084c159-4347-4d00-80aa-425f52565512)<br>
![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/d16fc493-ef23-430b-8737-895beb1f3614)<br>

## Summary and Conclusions of part 1 - running a trained machine learning model on Raspberry pi 4

By the creation of a custom dataset and Transfer Learning with YOLOv5, we managed to teach the machine to successfully detect flames in a video frame, while running on Raspberry pi 4 microcontroller.


## Autonomous Robot
In this section of the project, we developed the autonomousy of the robot, in C code (to insure **real time** behavior), while synchronising his actions with the trained model outputs.<br>

![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/b2858d92-37d2-4cc9-b3c9-6c270b8089ec)<br>


1. First, we cloned YOLOv5 repository to the Raspberry pi.
2. than, we installed all the requirments (packages) that YOLOv5 needs in order to operate.
3. We wrote a script, capture_img.py, that captures a single frame of the enviroment, through the webcam that is mount on the robot.
The image that is captured, is an 480x640 sized image.<br>
![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/6390634f-ca50-4cf0-9c13-24cfc98d2613)<br>

5. We excuted the detect.py script (of YOLOv5) which implies the Deep Learning alogrithm upon the single frame, and decides if there a flame in the image, where it is (with bounding box) and the level of confidence in the classification.

![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/364e1abb-6647-49eb-a8e3-57bfb51e1f22)<br>
![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/875f7380-6f1c-4ca0-b3e3-57c5a1d65d4d)

6. After YOLOV5 was classifying the object identity, it creats a text file which contains the object classification (represents as '0') and the four bounding box dimensions:<br>
![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/b3e0390c-2852-40f2-8782-a1cd804689cd)<br>

7. We wrote a code which implements an astrategy of the robot moving and dealing with obstacles and "empty" zones (when the robot sees nothing and has to scan the room in order to continue searching).

8. finally, we tested the robot in aspecific room, in different scenarios:<br>
  1. When it faces the flame directly with no obstacles.<br>
  2. When it has to encounter one obstacle before reaching the flame.<br>
  3. When it faces two obstacles or more before reaching the flame.<br>
  4. when it faces a wall.<br>
  5. when it faces a corner.<br>

two images that show the end of the robot searching and the classification:<br>
![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/57e53283-9e12-45ef-91ec-92c660ebfe13)<br>
![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/775c11f9-015f-4b39-9030-48c3233ed4fd)<br>
![image](https://github.com/AvivShuster/Fire-Detection-Final-Project/assets/121958931/c2c2cde2-2b3e-4ad5-8a7c-d80d32b4c93c)
