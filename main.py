from ultralytics import YOLO
import matplotlib.pyplot as plt
import serial.tools.list_ports
import cv2
import cvzone
import math
import time
import serial
import numpy as np
import threading
from utils import *


class ShotDetector:
    def __init__(self):
        # Load the YOLO model created from main.py - change text to your relative path
        self.model = YOLO("model.pt")
        self.class_names = ['ball', 'hoop']

        self.cap = None
        self.ball_pos = []  # array of tuples ((x_pos, y_pos), frame count, width, height, conf)
        self.hoop_pos = []  # array of tuples ((x_pos, y_pos), frame count, width, height, conf)

        self.frame_count = 0
        self.frame = None

        self.makes = 0
        self.attempts = 0

        # Used to detect shots (upper and lower region)
        self.up = False
        self.down = False
        self.up_frame = 0
        self.down_frame = 0


        self.original_width = None
        self.original_height = None
        self.serialInst=serial.Serial()
        self.stop_event = threading.Event()


    def run(self,frame,video):
        
        # Uncomment this when you use the LED ScoreBoard
        # self.serial_initialize()
        # command = f"start_timer,teamA,teamB,0,0"
        # self.serialInst.write(command.encode('utf-8'))

        self.cap = cv2.VideoCapture(video)

        self.original_width = int(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        self.original_height = int(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

        cv2.namedWindow(frame, cv2.WINDOW_NORMAL)
        cv2.resizeWindow(frame, self.original_width, self.original_height)
        while True:

            start_time = time.time()            

            ret, self.frame = self.cap.read()

            if not ret:
                # End of the video or an error occurred
                break

            results = self.model(self.frame, stream=True)

            for r in results:
                boxes = r.boxes
                for box in boxes:
                    # Bounding box
                    if len(box.xyxy) > 0:
                        x1, y1, x2, y2 = box.xyxy[0]
                        x1, y1, x2, y2 = int(x1), int(y1), int(x2), int(y2)
                    else:
                        continue

                    w, h = x2 - x1, y2 - y1

                    # Confidence
                    conf = math.ceil((box.conf[0] * 100)) / 100
                    # Class Name
                    cls = int(box.cls[0])
                    if cls < len(self.class_names):
                        current_class = self.class_names[cls]
                    else:
                    # Handle the case where cls is out of range
                        current_class = "Unknown Class"

                    #print("cls:", cls)


                    center = (int(x1 + w / 2), int(y1 + h / 2))

                    if conf > .5  and current_class == "ball":
                        self.ball_pos.append((center, self.frame_count, w, h, conf))
                        cv2.rectangle(self.frame, (x1, y1), (x2, y2), (0, 0, 255), 3) 

                    if conf > .5 and current_class == "hoop":
                        self.hoop_pos.append((center, self.frame_count, w, h, conf))
                        cv2.rectangle(self.frame, (x1, y1), (x2, y2), (0, 255, 0), 3) 
            self.clean_motion()
            self.shot_detection()
            self.display_score()
            self.frame_count += 1


            cv2.imshow(frame, self.frame)
            

            if cv2.waitKey(1) & 0xFF == ord('q'):  # higher waitKey slows video down, use 1 for webcam
                self.stop_event.set()

        
        #self.video_writer.release()
        self.cap.release()
        cv2.destroyWindow(frame)
        

    def clean_motion(self):
        # Clean and display ball motion
        self.ball_pos = clean_ball_pos(self.ball_pos, self.frame_count)
        for i in range(0, len(self.ball_pos)):
            cv2.circle(self.frame, self.ball_pos[i][0], 2, (0, 0, 255), 2)

        # Clean hoop motion and display current hoop center
        if len(self.hoop_pos) > 1:
            self.hoop_pos = clean_hoop_pos(self.hoop_pos)
            cv2.circle(self.frame, self.hoop_pos[-1][0], 2, (128, 128, 0), 2)

    def shot_detection(self):
        if len(self.hoop_pos) > 0 and len(self.ball_pos) > 0:
            # Detecting when ball is in 'up' and 'down' area - ball can only be in 'down' area after it is in 'up'
            if not self.up:
                self.up = detect_up(self.ball_pos, self.hoop_pos)
                if self.up:
                    self.up_frame = self.ball_pos[-1][1]

            if self.up and not self.down:
                self.down = detect_down(self.ball_pos, self.hoop_pos)
                if self.down:
                    self.down_frame = self.ball_pos[-1][1]

            # If ball goes from 'up' area to 'down' area in that order, increase attempt and reset
            if self.frame_count % 10 == 0:
                if self.up and self.down and self.up_frame < self.down_frame:
                    self.attempts += 1
                    self.up = False
                    self.down = False

                    if score(self.ball_pos, self.hoop_pos):
                        self.makes += 1
                        # Uncomment this when you use the LED ScoreBoard
                        # self.send_ard()

    def display_score(self):
        # Add text
        text = str(self.makes) + " / " + str(self.attempts)
        cv2.putText(self.frame, text, (50, 125), cv2.FONT_HERSHEY_SIMPLEX, 3, (255, 255, 255), 6)
        cv2.putText(self.frame, text, (50, 125), cv2.FONT_HERSHEY_SIMPLEX, 3, (0, 0, 0), 3)
    
    def serial_initialize(self):
        self.serialInst.baudrate = 9600
        self.serialInst.port = "COM" + str(4) # check and change the COM port
        self.serialInst.open()
    
    def send_ard(self):
        command = f"ignore,ignore,ignore,{self.makes},ignore"
        self.serialInst.write(command.encode('utf-8'))


if __name__ == "__main__":
    team1 = ShotDetector()
    team2 = ShotDetector()

    # path to the videos
    # use 1 instead of the path to use webcam
    vid1="inputvids\clg0.mp4"
    vid2="inputvids\clg0.mp4"

    t1=threading.Thread(target=team1.run , args=("t1",vid1))
    t2=threading.Thread(target=team2.run , args=("t2",vid2))

    t1.start()
    t2.start()

    t1.join()
    t2.join()

