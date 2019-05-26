#include "Wt61cUart.h"

//Get parameter from parameter service and initialize the other parameter.
WTU::Wt61cUart::Wt61cUart( ros::NodeHandle& nh){

	nh.getParamCached("uart_com", com_);
	nh.getParamCached("uart_baudrate",baudrate_);
	nh.getParamCached("g", g_);
	nh.getParamCached("PI", PI_);
	nh.getParamCached("topic_pub", topic_pub_);

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
	
		ser.setBaudrate(baudrate_*100);
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
		ROS_INFO_STREAM("The port initialize succeed.");
		return 0;
	}
	else
		return -1;

}

//read data function
int WTU::Wt61cUart::GetData () {

	if(ser.available()) {

		//read date from port
		ser.read(UartData_, ser.available());
		//test output a package
		/*ROS_INFO("/%04x/%04x/%04x/",UartData_[0], UartData_[1], UartData_[2]);
		ROS_INFO("/%04x/%04x/%04x/",UartData_[3], UartData_[4], UartData_[5]);
		ROS_INFO("/%04x/%04x/%04x/",UartData_[6], UartData_[7], UartData_[8]);
		ROS_INFO("/%04x/%04x/%04x/",UartData_[9], UartData_[10], UartData_[11]);*/
	}
	return 0;
}

//Analytical data function
int WTU::Wt61cUart::TranslateAndPub() {

	int i;
	sensor_msgs::Imu wt61c_imu;   //declare the pub message

	for (int i = 0; i < 3; ++i) {

		if(UartData_[i*11] == 0x55) {
			switch(UartData_[i*11+1]) {
				case 0x51:
				wt61c_imu.linear_acceleration.x = ((UartData_[i* 11+ 3]<< 8) | UartData_[i *11+ 2])/ 32768* 16* g_;
				wt61c_imu.linear_acceleration.y = ((UartData_[i* 11+ 5]<< 8) | UartData_[i *11+ 4])/ 32768* 16* g_;
				wt61c_imu.linear_acceleration.z = ((UartData_[i* 11+ 7]<< 8) | UartData_[i *11+ 6])/ 32768* 16* g_;

				break;
				case 0x52:
				wt61c_imu.angular_velocity.x = ((UartData_[i* 11+ 3]<< 8) | UartData_[i *11+ 2])/ 32768* 2000* PI_/180;
				wt61c_imu.angular_velocity.y = ((UartData_[i* 11+ 5]<< 8) | UartData_[i *11+ 4])/ 32768* 2000* PI_/180;
				wt61c_imu.angular_velocity.z = ((UartData_[i* 11+ 7]<< 8) | UartData_[i *11+ 6])/ 32768* 2000* PI_/180;
				break;
				case 0x53:
				wt61c_imu.orientation.x = ((UartData_[i* 11+ 3]<< 8) | UartData_[i *11+ 2])/ 32768* 2000* PI_/180;
				wt61c_imu.orientation.y = ((UartData_[i* 11+ 5]<< 8) | UartData_[i *11+ 4])/ 32768* 2000* PI_/180;
				wt61c_imu.orientation.z = ((UartData_[i* 11+ 7]<< 8) | UartData_[i *11+ 6])/ 32768* 2000* PI_/180;
				break;
			}
		}
		else
			break;
	}
	wt61c_pub_.publish(wt61c_imu);	
	ROS_INFO("The data has been pub.");	

	return 0;			
}


//check data function 
//if the data is right ,return true, wrong ,false
bool WTU::Wt61cUart::CheckData(){
	int pkg, index, sum;
	bool cheak_result;
	for (int pkg = 0; pkg < 3; )
	{
		sum = 0;
		for (int index = 0; index < 10; index++)
		{
			sum += UartData_[pkg*11+index];
		}
		if (UartData_[pkg*11+10] == sum%0x100)
			pkg++;
		else
			return false;
	}
	return true;
}