# Autonomous robotic fire detection using Deep Learning YOLOv5 algorithm and Rasberry pi 4

As the name suggests, this project includes two different parts:<br>
1. Creating a Fire Detection model, on Raspberry pi 4 (2GB RAM), using **custom dataset and Transfer Learning with YOLOv5 algorithm**.
2. Autonomous robot which drives to the target once it is detected.<br>

At this moment of writing, the fire detection part is completed and the second part is in the proccess of development.

## Custom Dataset

In this project, our mission is to detect flames of fire in an indoor space.
Therefore, We assembled a unique dataset which contains flames of candles and lighters, in different scenarios and lightning conditions.

This dataset contains XXX images with a size of YYxYY.<br>

The annotation process was executed with Roboflow, so in the end of the process, we had a custom yaml file in YOLOv5 format, which helps the model to know how to decide whether a flame is detected or not.<br>
With Roboflow, we managed to implement various augmentations upon the dataset, so that it is vary diversified.
Therefore, the model will be more resilient and capable of dealing with frames with high brightness, blur, different rotations and angles, concealments and noise.  

A summarization of the custom dataset generation:<br>



## Setup

1. Clone YOLOv5 base code and install requirements on the Raspberry pi:<br>
![image](https://user-images.githubusercontent.com/121958931/220930034-1bab3649-2123-40ce-bb31-117c3a29f37b.png)

2. Import the desired packages and connect to ROBOFLOW:<br>
![image](https://user-images.githubusercontent.com/121958931/220946259-a0781378-2e15-425e-a6b9-efd684b4fdde.png)

3. Train our custom YOLOv5 model:<br>
Important: **we trained the model on a strong computer with Google's GPU, and not on Raspberry pi due to his limited resources**<br>
![image](https://user-images.githubusercontent.com/121958931/220946339-efdf1f05-e620-462b-80b0-d50f10b13435.png)

The training process:<br>
![image](https://user-images.githubusercontent.com/121958931/220946945-5e259e5b-a29a-4661-8251-3977086355fb.png)

4. Run model with trained weights (**on Raspberry pi**):<br>
![image](https://user-images.githubusercontent.com/121958931/220947001-cea85f75-3e69-4576-8547-ac63973cc882.png)

## Results

![image](https://user-images.githubusercontent.com/121958931/220947210-6b664c42-1d42-4260-a4e3-e24b28288e21.png)
![image](https://user-images.githubusercontent.com/121958931/220947632-8951a910-a224-4ee4-bdf3-617c51f06733.png)
![image](https://user-images.githubusercontent.com/121958931/220947843-2894b777-cb2c-4bbf-9e4d-ebe4375aed00.png)
![image](https://user-images.githubusercontent.com/121958931/220947999-b8e73ff6-cad7-4a20-9674-24c089ca4f37.png)

## Summary and Conclusions of part 1 - running a trained machine learning model on Raspberry pi 4

By the creation of a custom dataset and Transfer Learning with YOLOv5, we managed to teach the machine to successfully detect flames in a video frame, while running on Raspberry pi 4 microcontroller.
In the future of this project, we will develop the autonomous of the robot, in C  (to insure **real time** behavior), while synchronising his actions with the trained model outputs.

