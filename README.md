# Fire-Detection-Final-Project
An autonomous robotic fire detection using Deep Learning

As the name suggests, this project includes two different parts:<br>
1. Fire Detection using custom dataset and Transfer Learning with YOLOv5 algorithm.
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

1. Clone YOLOv5 base code and install requirements:<br>
![image](https://user-images.githubusercontent.com/121958931/220930034-1bab3649-2123-40ce-bb31-117c3a29f37b.png)

2. Import the desired packages and connect to ROBOFLOW:<br>

3. Train our custom YOLOv5 model:<br>


The training process:<br>


4. Run model with trained weights:<br>


## Results



## Summary and Conclusions of part 1

By the creation of a custom dataset and Transfer Learning with YOLOv5, we managed to teach the machine to successfully detect flames in a video frame.

