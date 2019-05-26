#include "ros/ros.h"
#include "Wt61cUart.h"
#include "serial/serial.h"


int main(int argc, char** argv)
{
	ros::init(argc, argv, "Wt61cUart_node");

	ros::NodeHandle n;

	WTU::Wt61cUart wt61cuart(n);    //initilize the uart parameter
	
	wt61cuart.UartInit();       //declare the uart port
	
	ros::Rate loop_rate(100);

	while(ros::ok()){
		
		ros::spinOnce();

		wt61cuart.GetData();    //get date from uart port

		//check the data to ensure is right
		if( ! wt61cuart.CheckData()){    
			ROS_INFO("The date is wrong!");
			continue;
		}

		//pub the data to topic "orientation_acceleration"
		wt61cuart.TranslateAndPub();

		loop_rate.sleep();
	}

		

	return 0;
}