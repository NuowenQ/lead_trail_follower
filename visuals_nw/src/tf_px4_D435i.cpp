#include "ros/ros.h" //take away period in the beggining
#include "sensor_msgs/Imu.h"
#include "mavros_msgs/State.h"


sensor_msgs::Imu modified_msg;
ros::Publisher imu_pub;


void chatterCallback(const sensor_msgs::Imu::ConstPtr& msg)
{
 modified_msg = *msg;
 std::fill(modified_msg.orientation_covariance.begin(), modified_msg.orientation_covariance.end(), 0.0);
 modified_msg.orientation_covariance[0] = -1.0;
  for (int i = 0; i < 3; ++i) {
   modified_msg.angular_velocity_covariance[i * 3 + i] = 0.01;
 }


 for (int i = 0; i < 3; ++i) {
   modified_msg.linear_acceleration_covariance[i * 3 + i] = 0.01;
 }


 //ROS_INFO("Hello");
 //ROS_INFO("Imu Seq: [%d]", msg->header.seq);
 modified_msg.orientation.x = 0;
 modified_msg.orientation.y = 0;
 modified_msg.orientation.z = 0;
 modified_msg.orientation.w = 0;


 modified_msg.angular_velocity.x = -(msg->angular_velocity.y);
 modified_msg.angular_velocity.y = -(msg->angular_velocity.z);
 modified_msg.angular_velocity.z = msg->angular_velocity.x;


 modified_msg.linear_acceleration.x = -(msg->linear_acceleration.y);
 modified_msg.linear_acceleration.y = -(msg->linear_acceleration.z);
 modified_msg.linear_acceleration.z = msg->linear_acceleration.x;


 imu_pub.publish(modified_msg);
 //ROS_INFO("Imu modified Orientation x: [%f], y: [%f], z: [%f], w: [%f]", modified_msg.orientation.x,modified_msg.orientation.y,modified_msg.orientation.z,modified_msg.orientation.w);
 //ROS_INFO("Imu Orientation x: [%f], y: [%f], z: [%f], w: [%f]", msg->orientation.x,msg->orientation.y,msg->orientation.z,msg->orientation.w);


}


mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
   current_state = *msg;
}




int main(int argc, char **argv)
{
 ros::init(argc, argv, "imu_listener");


 ros::NodeHandle n;


 imu_pub = n.advertise<sensor_msgs::Imu>("processed_imu_data", 1000);
 ros::Subscriber state_sub = n.subscribe<mavros_msgs::State>
         ("mavros/state", 10, state_cb);
 ros::Subscriber sub = n.subscribe<sensor_msgs::Imu>
         ("mavros/imu/data", 1000, chatterCallback);




 ros::Rate rate(20.0);


 while(ros::ok() && current_state.connected){
     ros::spinOnce();
     rate.sleep();
 }


 for(int i = 100; ros::ok() && i > 0; --i){
     ros::spinOnce();
     rate.sleep();
 }


 ros::spin();


 return 0;
}


