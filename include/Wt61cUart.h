#ifndef WT61CUART_H
#define WT61CUART_H

#include <vector>
#include <math.h>
#include <time.h>

#include "ros/ros.h"
#include "serial/serial.h"
#include "std_msgs/String.h"
#include "std_msgs/Empty.h"
#include "sensor_msgs/Imu.h"
#include "tf/LinearMath/Quaternion.h"

#define PI 3.14159

namespace WTU {

	class Wt61cUart {
	private:
		int baudrate_,index_;
		std::string com_;
		serial::Serial ser;
		
		double g_;
		std::vector<uint8_t> UartData_;
		
		std::string topic_pub_;
		ros::Publisher wt61c_pub_;
	public:
		Wt61cUart(ros::NodeHandle&);
		~Wt61cUart();
		int TranslateAndPub();
		int UartInit();
		int GetAndCheck();
	};
}
#endif