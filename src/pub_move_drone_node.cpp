#include <ros/ros.h>
#include <std_msgs/String.h>
#include <stdio.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/GlobalPositionTarget.h>
#include <mavros_msgs/PositionTarget.h>
#include <geographic_msgs/GeoPoseStamped.h>
#include "geometry_msgs/PoseStamped.h"
#include "geometry_msgs/Vector3Stamped.h"
#include <sensor_msgs/NavSatFix.h>

//publisher
//geographic_msgs::GeoPoseStamped target_pose; //for global
geometry_msgs::PoseStamped target_pose; //for local
mavros_msgs::PositionTarget rotate_pose;

sensor_msgs::NavSatFix current_pose;

//subscriber
mavros_msgs::State current_state;

bool alt_flag = false;

void state_cb(const mavros_msgs::State::ConstPtr msg){
    ROS_INFO("Connection Alived!");
    current_state = *msg;
}

// void target_cb(const geographic_msgs::GeoPoseStamped msg){
//     target_pose = msg;
//     alt_flag = false;
//     //ROS_INFO("MOVE!\n");
//     ROS_INFO("Received(lat,long,alt): %4.2f, %4.2f, %4.2f\n",msg.pose.position.latitude, msg.pose.position.longitude, msg.pose.position.altitude);
// }

// void target_alt_cb(const geographic_msgs::GeoPoseStamped msg){
//     target_pose = msg;
//     alt_flag = true;
//     //ROS_INFO("MOVE!\n");
//     ROS_INFO("Received(alt): %4.2f\n", msg.pose.position.altitude);
// }
/*
void target_cb(const geometry_msgs::PoseStamped::ConstPtr msg){
    target_pose = *msg;
    //ROS_INFO("MOVE!\n");
    //ROS_INFO("Received(lat,long,alt): %4.2f, %4.2f, %4.2f\n",msg.pose.position.latitude, msg.pose.position.longitude, msg.pose.position.altitude);
}
*/

void global_cb(const sensor_msgs::NavSatFix msg ){
	current_pose = msg;
	ROS_INFO("Current(lat,long,alt): %4.2f, %4.2f, %4.2f\n",msg.latitude, msg.longitude, msg.altitude);
}

int main(int argc, char **argv)
{
   ros::init(argc, argv, "pub_setpoints");
   ros::NodeHandle n;

	//publishers
   //ros::Publisher move_pub = n.advertise<mavros_msgs::GlobalPositionTarget>("mavros/setpoint_raw/global",10);
   //ros::Publisher move_pub = n.advertise <geographic_msgs::GeoPoseStamped> ("mavros/setpoint_position/global", 1);
   ros::Publisher move_pub = n.advertise <geometry_msgs::PoseStamped> ("mavros/setpoint_position/local", 10); //for local
   //ros::Publisher rotate_pub = n.advertise <mavros_msgs::PositionTarget> ("mavros/setpoint_raw/local", 10);

   //subscribers
   //ros::Subscriber target_sub = n.subscribe<geographic_msgs::GeoPoseStamped>("targeting",1,target_cb); //for global
   //ros::Subscriber target_alt_sub = n.subscribe<geographic_msgs::GeoPoseStamped>("targeting_alt",1,target_alt_cb);
   //ros::Subscriber target_sub = n.subscribe<geometry_msgs::PoseStamped>("targeting",10,target_cb);
   ros::Subscriber state_sub = n.subscribe<mavros_msgs::State>("mavros/state", 1, state_cb);
   ros::Subscriber global_sub = n.subscribe<sensor_msgs::NavSatFix>("/mavros/global_position/global", 1, global_cb);

   //servicesClient
   ros::ServiceClient arming_client = n.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");
   ros::ServiceClient set_mode_client = n.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");

   ros::Rate rate(2);

    while(ros::ok() && !current_state.connected){
        ROS_INFO("Looped!");
        ros::spinOnce();
        rate.sleep();
    }
    target_pose.header.stamp = ros::Time::now();
    //target_pose.header.frame_id = 1;
    for (int i = 0;i<10;i++){
		// target_pose.pose.position.latitude = current_pose.latitude;
		// target_pose.pose.position.longitude = current_pose.longitude;
		// target_pose.pose.position.altitude = current_pose.altitude + 5;
		target_pose.pose.position.x = 0;
		target_pose.pose.position.y = 0;
		target_pose.pose.position.z = 2;
    ros::spinOnce();
    rate.sleep();
	}

	//rotate_pose.yaw = 0;
	//rotate_pose.yaw_rate = 1;


	//target_pose.latitude = current_pose.latitude;
    //target_pose.longitude = current_pose.longitude;
    //target_pose.altitude = current_pose.altitude + 5;

    //alt_pose.pose.position.altitude = current_pose.altitude + 5;


    //send a few setpoints before starting
    for(int i = 10; ros::ok() && i > 0; --i){
        move_pub.publish(target_pose);
        //alt_pub.publish(alt_pose);
        ros::spinOnce();
        rate.sleep();
    }

    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.custom_mode = "OFFBOARD";

    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;

   ros::Time last_request = ros::Time::now();

   while(ros::ok()){
       ROS_INFO("Target(lat,long,alt): %4.2f, %4.2f, %4.2f\n",target_pose.pose.position.x, target_pose.pose.position.y, target_pose.pose.position.z);
       target_pose.header.stamp = ros::Time::now();
       //rotate_pose.header.stamp = ros::Time::now();
       target_pose.header.frame_id = 1;
       //rotate_pose.header.frame_id = 1;


       
       if( current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - last_request > ros::Duration(5.0))){
            if( set_mode_client.call(offb_set_mode) &&
                offb_set_mode.response.mode_sent){
                ROS_INFO("Offboard enabled");
            }
            last_request = ros::Time::now();

             for(int i = 100; ros::ok() && i > 0; --i){
				move_pub.publish(target_pose);
				//alt_pub.publish(alt_pose);
			}
        } else if( !current_state.armed &&
                (ros::Time::now() - last_request > ros::Duration(5.0))){
                if( arming_client.call(arm_cmd) &&
                    arm_cmd.response.success){
                    ROS_INFO("Vehicle armed");
                }
                last_request = ros::Time::now();
                 for(int i = 100; ros::ok() && i > 0; --i){
					move_pub.publish(target_pose);
					//alt_pub.publish(alt_pose);
				}
            }
		else{

		   move_pub.publish(target_pose);
		   //rotate_pub.publish(rotate_pose);
			//alt_pub.publish(alt_pose);
        }
      


	   //count+=0.01;
	   //rotate_pose.yaw=count;

       //move_pub.publish(target_pose);
    //    if(alt_flag){
    //     target_pose.pose.position.latitude = current_pose.latitude;
	// 	target_pose.pose.position.longitude = current_pose.longitude;
    //    }
    //    move_pub.publish(target_pose);
       ros::spinOnce();
       rate.sleep();

   }
   return 0;
}
