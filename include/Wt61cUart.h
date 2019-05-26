#ifndef WT61CUART_H
#define WT61CUART_H

#include <vector>

#include "ros/ros.h"
#include "serial/serial.h"
#include "std_msgs/String.h"
#include "std_msgs/Empty.h"
#include "sensor_msgs/Imu.h"

namespace WTU {

	class Wt61cUart {
	private:
		int baudrate_;
		std::string com_;
		serial::Serial ser;
		
		double g_,PI_;
		std::vector<uint8_t> UartData_;
		
		std::string topic_pub_;
		ros::Publisher wt61c_pub_;
	public:
		Wt61cUart(ros::NodeHandle&);
		~Wt61cUart();
		bool CheckData();
		int TranslateAndPub();
		int UartInit();
		int GetData();
		
	};
}
#endif