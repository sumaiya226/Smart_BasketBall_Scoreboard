
# Smart-Basketball-Score-Board
This IoT-based system detects a basketball and the hoop in real time and determines if a shot results in a score. By analyzing object positions in real-time , the model can detect whether the ball went through the hoop and updates it on the Scoreboard.
## Features
- <ins>**Real-time Detection:**</ins> Uses YOLOv8 for fast and accurate detection of basketball and hoop.
- <ins>**Scoring Analysis:**</ins> Determines if the basketball successfully enters the hoop.
- <ins>**Scoreboard:**</ins> Score is updated on the LED Scoreboard as well as the webpage.
## Technologies Used

- <ins>**YOLOv8:**</ins> A machine learning model for Object detection for identifying the basketball and hoop.
- <ins>**Python:**</ins> Primary language for running detection scripts.
- <ins>**OpenCV:**</ins> Library used for video processing.
- <ins>**IoT Devices:**</ins> Arduino UNO, RF Module, wireless cameras, WS2812B LED (For scoreboard), Jetson Xavier (GPU).
## Setup

###  Clone the Repository
```bash
 git clone https://github.com/muzammil-ibrahim/Smart-Basketball-Score-Board
```
### Install the dependencies
```bash
  pip install -r requirements.txt
```
### To Run the program
- Download the input video and set the path, link to the video [click here](https://drive.google.com/file/d/1BdkBzDuWUNy4JURl1DnnEx4k9pT5q4WR/view?usp=drive_link).
- Ensure the YOLO model is in the directory.
- Setup the IoT devices (if u want to).
- Run main.py
- Check the webpage.
## Contributors
- [Afreed](https://github.com/mohd-afreed)
- [Ibrahim](https://github.com/muzammil-ibrahim)
- [Shahed](https://github.com/MOHAMMEDSHAHED786)
- [Sumaiya](https://github.com/sumaiya226)
- [Vaishnavi](https://github.com/vaishnavijade)
- [Vyahruti](https://github.com/Vyahruti)
