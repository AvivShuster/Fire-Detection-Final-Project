#include <stdio.h>
#include <python3.9/Python.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>

#define ENA_PIN 13
#define IN1_PIN 23
#define IN2_PIN 24
#define IN3_PIN 17
#define IN4_PIN 22
#define ENB_PIN 12

#define TRIG 11
#define ECHO 16

#define MAX_SPEED 450
#define MIN_DISTANCE 65 // centimeters
#define OBS_DISTANCE 65 // centimeters

#define forward set_motors(1,0,0,1)
#define backwards set_motors(0,1,1,0)
#define right set_motors(1,0,1,1) 
#define left set_motors(1,1,0,1)
#define rotate_left set_motors(0,1,0,1)
#define rotate_right set_motors(1,0,1,0)
#define stop set_motors(0,0,0,0)

void bypass_obstacle();
int move_when_no_obj(int time_cnt);

char * path = "/home/raspi/robot/yolov5/runs/detect/";
float distance = 0;
int obstacle_flag = 0;


typedef struct object_bbox {
    int object;
    float x_mid;
    float y_mid;
    float width;
    float height;
}object_bbox;

object_bbox detected_obj;

void INIT()

{
    wiringPiSetup();
    wiringPiSetupGpio();

    pinMode(IN1_PIN,OUTPUT);
    pinMode(IN2_PIN,OUTPUT);
    pinMode(ENA_PIN,PWM_OUTPUT);

    pinMode(IN3_PIN,OUTPUT);
    pinMode(IN4_PIN,OUTPUT);
    pinMode(ENB_PIN,PWM_OUTPUT);

}

void set_motors(int state_PIN1, int state_PIN2, int state_PIN3, int state_PIN4)
{
    digitalWrite(IN1_PIN, state_PIN1);
    digitalWrite(IN2_PIN, state_PIN2);
    digitalWrite(IN3_PIN, state_PIN3);
    digitalWrite(IN4_PIN, state_PIN4);

    pwmWrite(ENA_PIN, MAX_SPEED);
    pwmWrite(ENB_PIN, MAX_SPEED);
}

float read_distance()
{
    // Send Ultrasonic pulse
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(1000);
    digitalWrite(TRIG,LOW);

    // Wait for pleftulse to return
    clock_t pulse_start = 0;
    clock_t pulse_end = 0;

    while(digitalRead(ECHO) == LOW)
    {
        pulse_start = micros();
    }
    while(digitalRead(ECHO) == HIGH)
    { 
        pulse_end = micros();
    }

    // Calculate distance based on pulse duration
    float pulse_duration = (float)(pulse_end) - (float)(pulse_start);
    float distance = pulse_duration / 58.0;     // Time[us] * Velocity [cm/us] / 2 = Distance --> Distance = T[us]*(34400/2)[cm/s] = T[us]*17200[cm/s] = X/58
    return distance;
}

char *get_last_created_folder()
{
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    time_t last_time = 0;
    char* last_folder = NULL;

    // Open the "detect" directory
    dir = opendir(path); // returns a pointer to the detect folder
    if(dir == NULL)
    {
        printf("open detect directory failed");
        return NULL;
    }

    // Loop through the directory entries
    while((entry = readdir(dir)) != NULL) // returns a pointer to the folders inside the detect directory
    {
        char full_path[1024];
        sprintf(full_path, "%s%s", path, entry->d_name);
        if(stat(full_path, &st) == -1) // retrieve inforamation about the given path and check if it is valid
        {
            printf("stat failed");
            continue;
        }

        // Check if the entry is a directory
        if(S_ISDIR(st.st_mode))
        {
            // Check if the directory was created more recently than the current last folder
            if(difftime(st.st_mtime, last_time) > 0)
            {
                last_time = st.st_mtime; // temp variable that saves the last time the folder created
                if(last_folder != NULL)
                {
                    free(last_folder);
                }
                last_folder = strdup(full_path); // saves the path of the last created path
            }
        }
    }
    
    closedir(dir);
    
    if(last_folder == NULL) return NULL;
    else return last_folder;  
}

char * check_obj_existence(char * last_dir_created)
{
    char object_file_path[1024];
    static char line[50];

    sprintf(object_file_path,"%s/%s/%s",last_dir_created,"labels","img.txt");

    FILE * file;
    file = fopen(object_file_path, "r");
    if(file == NULL)
    {
        printf("fopen exp file failed");
        return NULL;
    }
    fgets(line, 50, file);
    
    fclose(file);
    return line;
}

object_bbox parse_object(char * line_to_parse)
{   
    const char s[2]=" ";
    char *token;
    object_bbox object_info;
    float obj_arr[5] = {};
    int cnt = 1;

    //Get the first token
    token = strtok(line_to_parse,s);
    object_info.object = atoi(token);

    // Walk through the rest of the tokens
    while(cnt < 5)
    {
        token = strtok(NULL, s);
        obj_arr[cnt] = atof(token);
        cnt++;
    }
    object_info.x_mid = obj_arr[1];
    object_info.y_mid = obj_arr[2];
    object_info.width = obj_arr[3];
    object_info.height = obj_arr[4];
    return object_info;
}

void object_capture()
{
    char *last_dir_created, *line_to_parse;
    char object_file_path[1024];
    object_bbox temp_obj;
    
    last_dir_created = get_last_created_folder();

    printf("last dir created: %s\n", last_dir_created);

    line_to_parse = check_obj_existence(last_dir_created);

    printf("Line to parse: %s\n", line_to_parse);

    if(line_to_parse != NULL)
    {
        temp_obj = parse_object(line_to_parse);

        if(temp_obj.object == 0)
        {
            printf("The object is: Fire.\n");
            printf("x_mid is: %f\n", temp_obj.x_mid);
            printf("y_mid is: %f\n", temp_obj.y_mid);
            printf("width is: %f\n", temp_obj.width);
            printf("height is: %f\n", temp_obj.height);
            detected_obj.object = temp_obj.object;
            detected_obj.x_mid = temp_obj.x_mid;
            detected_obj.y_mid = temp_obj.y_mid;
            detected_obj.width = temp_obj.width;
            detected_obj.height = temp_obj.height;
        }
        else printf("The object is: Not a fire.\n");
    }  
 
    else
    {
        printf("*\n");
        printf("*******No object detected.********\n"); // all values are 9 to know that its not an object
        printf("*\n");
        detected_obj.object = 9;
        detected_obj.x_mid = 9;
        detected_obj.y_mid = 9;
        detected_obj.width = 9;
        detected_obj.height = 9;
    }
        
       free(last_dir_created);
}

void capture_and_check()
{
    // capture frame
    system("python3 /home/raspi/robot/yolo_captures/capture_img.py");
    // YOLOv5 detection on the frame
    system("python3 /home/raspi/robot/yolov5/detect.py --weights /home/raspi/robot/yolov5/fire_prod.pt --img 640 --conf 0.5 --save-txt --source /home/raspi/robot/img.png");
    object_capture();
}

void bypass_obstacle()
{
    int returned_flag = 0, i=0;

    printf("*\n");
    printf("*******Starting bypass********\n");
    printf("*\n");

    distance = read_distance();
    delayMicroseconds(100000); // 0.1 sec
    printf("Distance is %f \n", distance);
    while(distance < OBS_DISTANCE) //corrections to the right
    {
        right; 
        delayMicroseconds(100000); // 0.1 sec
        stop;
        delayMicroseconds(200000); // 0.2 sec
        distance = read_distance();
        delayMicroseconds(100000); // 0.1 sec
        printf("Distance is %f \n", distance);
    } 
    right; // an extra correction
    delayMicroseconds(400000); // 0.4 sec
    stop;
    delayMicroseconds(200000); // 0.2 sec

    distance = read_distance();
    delayMicroseconds(100000); // 0.1 sec
    printf("Distance is %f \n", distance);

    if(distance > OBS_DISTANCE) 
    {
        printf("*\n");
        printf("*******going forward to the right, to bypass.********\n");
        printf("*\n");
        
        forward; // Using forward, because we checked that the minimum distance is large enough so it will enable us to move freely without clashing anything.
        delayMicroseconds(500000); // 0.5 sec

        stop;
        delayMicroseconds(200000); // 0.2 sec 

        left;
        delayMicroseconds(700000); // 0.7 sec 
        
        stop;
        delayMicroseconds(200000); // 0.2 sec 

        distance = read_distance();
        delayMicroseconds(100000); // 0.1 sec
        printf("Distance is %f \n", distance);

        if(distance < MIN_DISTANCE) 
        {
            returned_flag = 1;
            printf("*\n");
            printf("*******changing direction to the other side.********\n");
            printf("*\n");
            distance = read_distance();
            delayMicroseconds(100000); // 0.1 sec
            printf("Distance is %f \n", distance);
            while(distance < OBS_DISTANCE) // corrections to the left
            {
                left; 
                delayMicroseconds(100000); // 0.1 sec
                stop;
                delayMicroseconds(200000); // 0.2 sec
                distance = read_distance();
                delayMicroseconds(100000); // 0.1 sec
                printf("Distance is %f \n", distance);
            } 
            left; // an extra correction
            delayMicroseconds(500000); // 0.5 sec
            stop;
            delayMicroseconds(200000); // 0.2 sec     
        }
        else 
        {
            returned_flag = 0;

            printf("*\n");
            printf("*******bypass from the right. ********\n");
            printf("*\n");
            obstacle_flag = move_when_no_obj(8); // 0.8 sec

            if(obstacle_flag == 1) // it must be another obstacle because it has moved forword a little
            {
                capture_and_check();
                if(detected_obj.object == 0 && detected_obj.height > 0.1)
                {
                    printf("******Standing in front of the fire!!********\n");
                    printf("*\n");
                    for(i=0; i<20; i++)
                    {
                        backwards; 
                        delayMicroseconds(100000); // 0.1 sec
                        forward;
                        delayMicroseconds(100000); // 0.1 sec
                    }
                    stop;
                    delayMicroseconds(200000); // 0.1 sec
                    exit(0);
                }
                else
                {
                    bypass_obstacle();
                    returned_flag = 0;
                }
            }  
            
            else
            {        
                printf("*\n");
                printf("*******bypass completed !!! ********\n");
                printf("*\n");
            }
        }
    }
    else 
    {
        printf("*\n");
        printf("*******Escape from corner********\n");
        printf("*\n");
        right;
        delayMicroseconds(1050000); // 1.05 sec // smaller than 180 degrees
        
        stop;
        delayMicroseconds(200000); // 0.2 sec

        obstacle_flag = move_when_no_obj(20); // 2 sec 

    }
    
    if(returned_flag == 1) // going left to perform a bypass from the left
    {
        printf("*\n");
        printf("*******going forward to the left, to bypass.********\n");
        printf("*\n");

        obstacle_flag = move_when_no_obj(10); // 1 sec // going to the other side of it

        if(obstacle_flag == 1) // it encountered an obstacle / a corner, needs to escape
        {
            printf("*\n");
            printf("*******A corner or an obstacle.********\n");
            printf("*\n");
            
            left;
            delayMicroseconds(300000); // 0.3 sec // 45 degrees left
        
            stop;
            delayMicroseconds(200000); // 0.2 sec
            
            obstacle_flag = move_when_no_obj(20); // 2 sec 
        }  

        else
        {
            distance = read_distance();
            delayMicroseconds(100000); // 0.1 sec
            printf("Distance is %f \n", distance);

            if(distance > OBS_DISTANCE)
            {
                right;
                delayMicroseconds(700000); // 0.7 sec 

                stop;
                delayMicroseconds(200000); // 0.2 sec

                distance = read_distance();
                delayMicroseconds(100000); // 0.1 sec
                printf("Distance is %f \n", distance);

                if(distance < MIN_DISTANCE)
                {
                    returned_flag = 2;
                }
                else
                {
                    forward; 
                    delayMicroseconds(1000000); // 1 sec
                    stop;
                    delayMicroseconds(200000); // 0.2 sec
                    printf("*\n");
                    printf("*******bypass completed from the other side********\n");
                    printf("*\n");
                    returned_flag = 0;
                }
            }
            else 
            {
                returned_flag = 2;
            }
        }
    }
    if (returned_flag == 2)
    {
        printf("*\n");
        printf("*******Escape from a wall********\n");
        printf("*\n");
        
        right;
        delayMicroseconds(1350000); // 1.35 sec // 180 degrees

        stop;
        delayMicroseconds(200000); // 0.2 sec
        
        obstacle_flag = move_when_no_obj(20); // 2 sec
    }
}

int move_when_no_obj(int time_cnt)
{
    int cnt = 0;

    distance = read_distance();
    delayMicroseconds(100000); // 0.1 sec
    printf("Distance is %f \n", distance);

    while(distance > MIN_DISTANCE)
    {
        distance = read_distance();
        delayMicroseconds(100000); // 0.1 sec
    
        forward; 

        cnt++;

        if(cnt == time_cnt) 
        {
            stop;
            delayMicroseconds(200000); // 0.2 sec
            break;
        }
    }
    if(cnt < time_cnt)
    {
        printf("*\n");
        printf("*******stoped before an obstacle********\n");
        printf("*\n");
        stop;
        delayMicroseconds(200000); // 0.2 sec
        return 1; // detected obstacle
    } 
    else 
    {
        printf("*\n");
        printf("*******no obstacle********\n");
        printf("*\n");
        stop;
        delayMicroseconds(200000); // 0.2 sec
        return 0; // not detected obstacle
    }   
}

int first_scan()
{
    int i;
    capture_and_check();
    printf("*\n");
    printf("*******obj=%d********\n",detected_obj.object);
    printf("*\n");
    if(detected_obj.object != 0)
    {
        printf("*\n");
        printf("*******rotate right (first angle)********\n");
        printf("*\n");
        right;
        delayMicroseconds(200000); // 0.2 sec (first angle)
        stop;
        delayMicroseconds(200000); // 0.2 sec
        capture_and_check();
        printf("*\n");
        printf("*******obj=%d********\n",detected_obj.object);
        printf("*\n");
        if(detected_obj.object != 0)
        {
            printf("*\n");
            printf("*******rotate left to the other side (second angle)********\n");
            printf("*\n");
            left;
            delayMicroseconds(400000); // 0.4 sec (second angle)
            stop;
            delayMicroseconds(200000); // 0.2 sec
            capture_and_check();
            printf("*\n");
            printf("*******obj=%d********\n",detected_obj.object);
            printf("*\n");
            if(detected_obj.object != 0)
            {
                printf("*\n");
                printf("*******rotate right to the middle (no fire found in both angles)********\n");
                printf("*\n");
                right;
                delayMicroseconds(200000); // 0.2 sec 
                stop;
                delayMicroseconds(200000); // 0.2 sec
                return 0; // did not find object
            }
            else return 1;
        }
        else return 1;
    }
    else return 1;
}


void focus_left()
{
    // this for focusing from close distance
    if(detected_obj.height > 0.05 && detected_obj.x_mid <= 0.35) // focusing only if the obs is really on the left
    {
        printf("*\n");
        printf("******Focusing to the left from close distance!!********\n");
        printf("*\n");
        left;
        delayMicroseconds(250000); // 0.25 sec in order to focusing from colse!!!
        stop;
        delayMicroseconds(200000); // 0.2 sec
    }
    // this for focusing from far distance
    else if(detected_obj.height < 0.05 && detected_obj.x_mid <= 0.35) // focusing only if the obs is really on the left
    {
        printf("*\n");
        printf("******Focusing to the left from far distance!!********\n");
        printf("*\n");
        left;
        delayMicroseconds(200000); // 0.2 sec in order to focus from far!!!!
        stop;
        delayMicroseconds(200000); // 0.2 sec
    }
    else
    {
        printf("*\n");
        printf("******No focusing to left is needed!!********\n");
        printf("*\n");
    }
}

void focus_right()
{
    // this for focusing from close distance
    if(detected_obj.height > 0.05 && detected_obj.x_mid >= 0.65) // focusing only if the obs is really on the right
    {
        printf("*\n");
        printf("******Focusing to the right from close distance!!********\n");
        printf("*\n");
        right;
        delayMicroseconds(250000); // 0.25 sec in order to focus from close!!!
        stop;
        delayMicroseconds(200000); // 0.2 sec
    }
    // this for focusing from far distance
    else if(detected_obj.height < 0.05 && detected_obj.x_mid >= 0.65) // focusing only if the obs is really on the right
    {
        printf("*\n");
        printf("******Focusing to the right from far distance!!********\n");
        printf("*\n");
        right;
        delayMicroseconds(200000); // 0.2 sec in order to focus from far!!!!
        stop;
        delayMicroseconds(200000); // 0.2 sec
    }
    else 
    {
        printf("*\n");
        printf("******No focusing to right is needed!!********\n");
        printf("*\n");
    }
}

void scan_env()
{
    int i;
    for(i=0;i<8;i++)
    {
        capture_and_check();
        printf("*\n");
        printf("*******obj=%d********\n",detected_obj.object);
        printf("*\n");
        if(detected_obj.object != 0)
        {            
            printf("*\n");
            printf("*******moved left********\n");
            printf("*\n");
            delayMicroseconds(200000); // 0.2 sec
            left;
            delayMicroseconds(300000); // 0.3 sec (8 angles for ~270 deg)
            stop;
            delayMicroseconds(200000); // 0.2 sec
            printf("*\n");
            printf("******i=%d********\n",i);
            printf("*\n");
            if (i>=7)
            {
                printf("*\n");
                printf("starting to move forward to a different position\n");
                printf("*\n");
                obstacle_flag = move_when_no_obj(15); // 1.5 sec
                if(obstacle_flag == 1) // it must be an obstacle. stopped before 1.5 sec
                {
                    printf("*\n");
                    printf("need to bypass obstacle\n");
                    printf("*\n");
                    bypass_obstacle(); // the robot recognised an obstacle before moving forward for 1.5 sec.
                }
                i=-1; // next iteration, it will start from 0 
            }
        }
        else
        {
            if(detected_obj.height > 0.1) // we decided that 0.1 is the height in which the flame is large enough to be declared as a closed object
            {
                focus_left();
                focus_right();
                printf("******Standing in front of the fire!!********\n");
                printf("*\n");
                for(i=0; i<20; i++)
                {
                    backwards; 
                    delayMicroseconds(100000); // 0.1 sec
                    forward;
                    delayMicroseconds(100000); // 0.1 sec
                }
                stop;
                delayMicroseconds(200000); // 0.2 sec
                exit(0); 
            }
            else break;
        } 
    }
}
void main()
{
    INIT();
    pinMode(TRIG,OUTPUT);
    pinMode(ECHO,INPUT);
    int i = 0, flag_scan = 0; 

    stop;
    delayMicroseconds(20000); // 0.02 sec

    flag_scan = first_scan();
    if(flag_scan == 0) // no flame detected
    {
        obstacle_flag = move_when_no_obj(30); // 3 sec
        if(obstacle_flag == 1)
        {
            bypass_obstacle();
        }
        scan_env();
    }
    else
    {
        if(detected_obj.height > 0.1)
        {
            focus_left();
            focus_right();

            printf("******Standing in front of the fire!!********\n");
            printf("*\n");
            for(i=0; i<20; i++)
            {
                backwards; 
                delayMicroseconds(100000); // 0.1 sec
                forward;
                delayMicroseconds(100000); // 0.1 sec
            }
            stop;
            delayMicroseconds(200000); // 0.2 sec
            exit(0);
        }
        else
        {
            focus_left();
            focus_right();
            obstacle_flag = move_when_no_obj(30); // 3 sec
            if(obstacle_flag == 1)
            {
                bypass_obstacle();
            }
            scan_env();
        }
    }

    while(1)
    {
        //Object was detected in the specific angle 
        if(detected_obj.x_mid < 0.45 && detected_obj.x_mid != 9) 
        {
            focus_left();
            obstacle_flag = move_when_no_obj(30); // 3 sec
            
            if(obstacle_flag == 1) // stopped before 3 sec
            {
                if(detected_obj.x_mid < 0.45) focus_left();
                else focus_right();

                printf("*\n");
                printf("******An obstacle/object was found ahead !!********\n");
                printf("*\n");
                capture_and_check();
                if(detected_obj.object == 0 && detected_obj.height > 0.1) // object is close 
                {
                    printf("*\n");
                    printf("******Standing in front of the fire!!********\n");
                    printf("*\n");
                    for(i=0; i<20; i++)
                    {
                        backwards; 
                        delayMicroseconds(100000); // 0.1 sec
                        forward;
                        delayMicroseconds(100000); // 0.1 sec
                    }
                    stop;
                    delayMicroseconds(200000); // 0.2 sec
                    exit(0); 
                }
                else // it must be an obstacle
                {
                    bypass_obstacle(); // the robot recognised an obstacle before moving forward for 1.5 sec.
                    scan_env();
                }
            }
            else 
            {
                printf("*\n");
                printf("******Did not find the object.********\n");
                printf("*\n");
                capture_and_check(); 
                if(detected_obj.object != 0) scan_env();
            } 
        }

        else if(detected_obj.x_mid > 0.55 && detected_obj.x_mid != 9)
        {
            focus_right();
            obstacle_flag = move_when_no_obj(30); // 3 sec
            
            if(obstacle_flag == 1) // stopped before 3 sec
            {
                capture_and_check();
                if(detected_obj.object == 0 && detected_obj.height > 0.1) // object is closed 
                {
                    if(detected_obj.x_mid > 0.55) focus_right();
                    else focus_left();

                    printf("*\n");
                    printf("******Standing in front of the fire!!********\n");
                    printf("*\n");
                    for(i=0; i<20; i++)
                    {
                        backwards; 
                        delayMicroseconds(100000); // 0.1 sec
                        forward;
                        delayMicroseconds(100000); // 0.1 sec
                    } 
                    stop;
                    delayMicroseconds(200000); // 0.2 sec
                    exit(0);
                }
                else // it must be an obstacle
                {
                    bypass_obstacle(); // the robot recognised an obstacle before moving forward for 1.5 sec.
                    scan_env();
                }
            }
            else 
            {
                printf("*\n");
                printf("******Did not find the object.********\n");
                printf("*\n");
                capture_and_check();
                if(detected_obj.object != 0) scan_env();
            } 
        } 

        else if(detected_obj.x_mid > 0.45 && detected_obj.x_mid < 0.55 && detected_obj.x_mid != 9)
        {
            printf("*\n");
            printf("******No focusing is needed!!********\n");
            printf("*\n");
            obstacle_flag = move_when_no_obj(30); // 3 sec
            
            if(obstacle_flag == 1) // stopped before 3 sec
            {
                capture_and_check();
                if(detected_obj.object == 0 && detected_obj.height > 0.1) // object is closed 
                {
                    if(detected_obj.x_mid > 0.55) focus_right();
                    else focus_left();

                    printf("*\n");
                    printf("******Standing in front of the fire!!********\n");
                    printf("*\n");
                    for(i=0; i<20; i++)
                    {
                        backwards; 
                        delayMicroseconds(100000); // 0.1 sec
                        forward;
                        delayMicroseconds(100000); // 0.1 sec
                    } 
                    stop;
                    delayMicroseconds(200000); // 0.2 sec
                    exit(0);
                }
                else // it must be an obstacle
                {
                    bypass_obstacle(); // the robot recognised an obstacle before moving forward for the while 5 sec.
                    scan_env();                
                }
            }
            else 
            {
                printf("*\n");
                printf("******Did not find the object.********\n");
                printf("*\n");
                capture_and_check();
                if(detected_obj.object != 0) scan_env();
            } 
        }
        else
        {
            printf("*\n");
            printf("******Starting scan_env again********\n");
            printf("*\n");
            scan_env();
        }
    }
}
