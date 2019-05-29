#include "ros/ros.h"
#include "sensor_msgs/Imu.h"

#include <string.h>

void testCallback(const sensor_msgs::Imu::ConstPtr& wt61c){
	ROS_INFO("I heard :[%d]",wt61c);
}

int main (int argc,char** argv){

	ros::init(argc, argv, "test_sub");

	ros::NodeHandle nh;

	std::string topic_sub;

	nh.getParamCached("topic_pub", topic_sub);
	ros::Subscriber sub = nh.subscribe(topic_sub, 1000, testCallback);

	ros::spin();
	return 0;
}