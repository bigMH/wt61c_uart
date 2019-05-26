# Wt61c
## Environment
Ubuntu: 18.04  
ROS: melodic
## Version
Wt61c_Uart 1.0.0
## Install
1. Install the dependence package.
```
sudo apt-get install ros-melodic-serial
```
The introduce of package serial: http://wiki.ros.org/serial  
The serial::Serial Class Reference: docs.ros.org/jade/api/serial/html/classserial_1_1Serial.html  
2.Clone the package to your workspace:
```
cd src
git clone https://github.com/bigMH/wt61c.git
```
3. Make the wt61c connected with you computer and get the port you used.
```
cd /dev
ls ttyUSB*
sudo chmod a+rx ttyUSB*
```
Now the port listed is wuich you used.   
Open the wt61c_uart_bringup.launch in the launch folder,and replace the `/dev/ttyUSB0` with the port you used.  
4. Run the launch
```
roslaunch wt61c_uart wt61c_uart_bringup.launch
rostopic echo topic_pub
```
