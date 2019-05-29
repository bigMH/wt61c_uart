#include "Wt61cUart.h"
#include "tf/LinearMath/Quaternion.h"

#include <time.h>
//Get parameter from parameter service and initialize the other parameter.
WTU::Wt61cUart::Wt61cUart( ros::NodeHandle& nh){

	nh.getParamCached("/sensor_uart/uart_com", com_);
	nh.getParamCached("/sensor_uart/uart_baudrate",baudrate_);
	nh.getParamCached("/sensor_uart/g", g_);
	nh.getParamCached("topic_pub", topic_pub_);
	index_ = 0;

	//delcare the pub object
	wt61c_pub_ = nh.advertise<sensor_msgs::Imu>(topic_pub_, 1);

}
//
WTU::Wt61cUart::~Wt61cUart(){

}

		//initialize the UART port.
int WTU::Wt61cUart::UartInit() {
	try{
		
		ser.setPort(com_);
	
		ser.setBaudrate(baudrate_);
		serial::Timeout to = serial::Timeout::simpleTimeout(1000);
		ser.setTimeout(to);
		ser.open();                           // try to open the port
	}
	catch(serial::IOException& e){
		ROS_ERROR_STREAM("Unable to open port. Please try again.");
		return -1;
	}
	//Detects if the port is open
	if(ser.isOpen()) {
		//ser.flushInput();
		ROS_INFO_STREAM("The port initialize succeed.");
		ser.flushInput();
		sleep(0.1);
		return 0;
	}
	else
		return -1;

}


// read data function
int WTU::Wt61cUart::GetAndCheck() {

	int i,j;
	int sum = 0x55;

	while(UartData_.size()-index_<33){
		while(ser.available()<33){ROS_INFO("wait");}
		ser.read(UartData_,ser.available());
	}
	while(true){
		if(UartData_[index_] ==0x55 & UartData_[index_+1] ==0x51){
			//SRC check
			for (i= 1; i<10; i++)
				sum+= UartData_[index_+i];
			if(UartData_[index_+10] == sum%0x100)
				j = 1;
			sum = 0x55;
			for (i= 12; i<21; i++)
				sum+= UartData_[index_+i];
			if(UartData_[index_+21] == sum%0x100)
				j++;
			sum = 0x55;
			for (i= 23; i<32; i++)
				sum+= UartData_[index_+i];
			if(UartData_[index_+32] == sum%0x100)
				j++;
			if (j = 3){
				ROS_INFO("Yes,I got a complete package.");
				return 0;
			}
			else{
				sum = 0x55;
				index_++;
			}
		}
		else
			index_++;
		while(UartData_.size()-index_-32<33){
			while(ser.available()<33){}
			ser.read(UartData_,ser.available());
		}
	}	
}

// translate UartDate to Imu date,and pub
int WTU::Wt61cUart::TranslateAndPub(){
	sensor_msgs::Imu wt61c_imu;                //declare the pub message
	double linear_acceleration[2],angular_velocity[2],orientation[2];

	wt61c_imu.header.stamp = ros::Time::now();
	wt61c_imu.header.frame_id = "wt61c_uart";
	tf::Quaternion quate;

	linear_acceleration[0] = ( short (UartData_[index_+ 3]<< 8 | UartData_[index_+ 2]))/ 32768.0* 16.0* 9.8;
	linear_acceleration[1] = ( short (UartData_[index_+ 5]<< 8 | UartData_[index_+ 4]))/ 32768.0* 16.0* 9.8;
	linear_acceleration[2] = ( short (UartData_[index_+ 7]<< 8 | UartData_[index_+ 6]))/ 32768.0* 16.0* 9.8;
	wt61c_imu.linear_acceleration.x = linear_acceleration[0];
	wt61c_imu.linear_acceleration.y = linear_acceleration[1];
	wt61c_imu.linear_acceleration.z = linear_acceleration[2];
	/*ROS_INFO("wt61c_imu.linear_acceleration.x = %f", wt61c_imu.linear_acceleration.x);
	ROS_INFO("wt61c_imu.linear_acceleration.y = %f", wt61c_imu.linear_acceleration.y);
	ROS_INFO("wt61c_imu.linear_acceleration.z = %f", wt61c_imu.linear_acceleration.z);*/
	
	angular_velocity[0] = (short (UartData_[index_+ 14]<< 8 | UartData_[index_+ 13]))/ 32768.0* 2000* PI/180;
	angular_velocity[1] = (short (UartData_[index_+ 16]<< 8 | UartData_[index_+ 15]))/ 32768.0* 2000* PI/180;
	angular_velocity[2] = (short (UartData_[index_+ 18]<< 8 | UartData_[index_+ 17]))/ 32768.0* 2000* PI/180;
	wt61c_imu.angular_velocity.x = angular_velocity[0];
	wt61c_imu.angular_velocity.y = angular_velocity[1];
	wt61c_imu.angular_velocity.z = angular_velocity[2];
	/*ROS_INFO("wt61c_imu.angular_velocity.x = %f", wt61c_imu.angular_velocity.x);
	ROS_INFO("wt61c_imu.angular_velocity.y = %f", wt61c_imu.angular_velocity.y);
	ROS_INFO("wt61c_imu.angular_velocity.z = %f", wt61c_imu.angular_velocity.z);*/
	
	orientation[0] = (short(UartData_[index_+ 25]<< 8 | UartData_[index_+ 24]))/ 32768.0* 2000* PI/180;
	orientation[1] = (short(UartData_[index_+ 27]<< 8 | UartData_[index_+ 26]))/ 32768.0* 2000* PI/180;
	orientation[2] = (short(UartData_[index_+ 29]<< 8 | UartData_[index_+ 28]))/ 32768.0* 2000* PI/180;
	/*wt61c_imu.orientation.x = sin(orientation[0]/2)* cos(orientation[1]/2)* cos(orientation[2]/2)- cos(orientation[0]/2)* sin(orientation[1]/2)* sin(orientation[2]/2);
	wt61c_imu.orientation.y = sin(orientation[0]/2)* cos(orientation[1]/2)* sin(orientation[2]/2)+ cos(orientation[0]/2)* sin(orientation[1]/2)* cos(orientation[2]/2);
	wt61c_imu.orientation.z = cos(orientation[0]/2)* cos(orientation[1]/2)* sin(orientation[2]/2)- sin(orientation[0]/2)* sin(orientation[1]/2)* cos(orientation[2]/2);
	wt61c_imu.orientation.w = cos(orientation[0]/2)* cos(orientation[1]/2)* cos(orientation[2]/2)+ sin(orientation[0]/2)* sin(orientation[1]/2)* sin(orientation[2]/2);*/
	quate.setRPY(orientation[0], orientation[1], orientation[2]);
	wt61c_imu.orientation.x = quate[0];
	wt61c_imu.orientation.y = quate[1];
	wt61c_imu.orientation.z = quate[2];
	wt61c_imu.orientation.w = quate[3];
	/*ROS_INFO("wt61c_imu.orientation.x = %f", wt61c_imu.orientation.x);
	ROS_INFO("wt61c_imu.orientation.y = %f", wt61c_imu.orientation.y);
	ROS_INFO("wt61c_imu.orientation.z = %f", wt61c_imu.orientation.z);
	ROS_INFO("wt61c_imu.orientation.w = %f", wt61c_imu.orientation.w);*/

	wt61c_pub_.publish(wt61c_imu);

	//delate the old date	
	index_ =index_+ 32;
	UartData_.erase(UartData_.begin(),UartData_.begin()+index_);
	index_ = 0;
	ROS_INFO("The data has been pub.");	

	return 0;			
}


	