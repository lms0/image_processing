# image_processing
Color tracking for measuring angles between figures

C++/OpenCV program for detection of colored markers and angle measurement. The program opens a menu that allows for color calibration. Once the colors are detected, the user can select three markers of interests
and compute the angle defined by the points that correspond to the center of each marker. This can be helpful for doctors that need to measure the angle of a leg or arm of a patient that suffers a locomotive limitation.
This program allows you to watch a video and pause it at moments of interest, to perform operations of union and angle calculation between objects of a colour to be calibrated.
 
 Instructions for use:
 
 "./[executable name] [path to video] [optional: different output folder] EXAMPLE1: ./ejec /home/...video.mp4 EXAMPLE 2: ./ejec /home/...video.mp4 /home/.../OutputFolder/."
 
 By default, the video runs to completion. At any time, the following keys can be pressed to pause the video and perform a certain action:
 
- Key 1 *: Enter Calibration Mode. The Hue, Saturation and Value ranges are adjusted, and the result is displayed as a binary image.
 
- Enter key *: Exits Calibration Mode. The filter values are saved in a file, which will be loaded each time the program is run,
 to avoid having to calibrate every time you start the program.
 
- Spacebar key *: Enters/Exits the Point Join Mode. Click (left click) near 3 objects of interest and draw lines between the centres of each object. 
 
 The angle of interest will be displayed and a screenshot and a .csv file with the name and angles will be saved in the containing folder.
 The video can be resumed by exiting Point Join Mode and repeating the process for a new frame of the video.

![alt text](https://github.com/lms0/image_processing/blob/main/Ariel1.jpg)
