# Suction Experiment
Experiment to test the performance of the suction cup when cartesian and angular noise is added to its position w.r.t. the center of a sphere. This matters for our gripper design, in order to build better insights of the dimensions used in the gripper.


# Basic Installation

The following steps were performed under Ubuntu 20.04.5 LTS (Focal Fossa)(https://www.releases.ubuntu.com/focal/)

### Arduino  
1. Install Arduino  
2. Search and install the libraries **rosserial** and **adafruit mprls**  
3. If after compiling you encounter issues with *cstring*, try:  
open **msg.h**  
replace `#include <cstring>` with `#include<string.h>`  
replace `std::memcpy()` with `memcpy()` 
     
### ROS
1. Install ROS [noetic](http://wiki.ros.org/noetic/Installation/Ubuntu)
2. Create WorkSpace  
```console
mkdir -p your_ws/src && cd catkin_ws
```
3. Don't forget to source the workspace   
`gedit ~/.bashrc` and add this line at the end `source ~/<your_ws>/devel/setup.bash` 

### ROS - Serial

4. Install **rosserial** package  
```console
cd <your_ws>/src  
git clone https://github.com/ros-drivers/rosserial.git  
cd <your_ws>  
catkin_make
```

### ROS - Camera
Install **opencv** or **usb_cam**. The latter allows to adjust color parameters (e.g. saturation, contrast, brightness)

5. Install **opencv** package
```console
cd <your_ws>/src 
git clone https://github.com/ros-drivers/video_stream_opencv
cd <your_ws>
catkin_make
```

6. Install **usb_cam** package if you have issues with Color
```console
cd <your_ws>/src
git clone https://github.com/ros-drivers/usb_cam 
cd <your_ws>
catkin_make
```

### ROS - MoveIt

7. Install **moveit** package
```console
sudo apt install ros-noetic-moveit
```

# Running
1. Upload code into **arduino board**.
2. Check the port name, and make sure it matches the one written at line 50 of **serial_node.py**.
3. Launch the lab setup in 1st terminal:
* With real hardware (ur5, camera, arduino)
```console
roslaunch suction-experiment suctioncup_experiment.launch
```
* Or just to visualize in RVIZ
```console
roslaunch apple_proxy pickApp.launch
```

4. Run experiment code in 2nd terminal:
```console
python3 suction_experiment.py
 ```

## Tips  
If you want to read a certain sensor/topic from command line:
```console
rostopic echo /gripper/pressure
```
Also, if you want to send a control command from command line:
```console
rosservice call openValve
```
or
```console
rosservice call closeValve
```

Tips for the camera:
https://askubuntu.com/questions/348838/how-to-check-available-webcams-from-the-command-line