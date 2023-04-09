/**
 * @file offb_node.cpp
 * @brief offboard example node, written with mavros version 0.14.2, px4 flight
 * stack and tested in Gazebo SITL
 */

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include "math.h"
#include <std_msgs/String.h>
#include <stdio.h>
#include <mavros_msgs/GlobalPositionTarget.h>
#include <mavros_msgs/PositionTarget.h>
#include <geographic_msgs/GeoPoseStamped.h>
#include "geometry_msgs/Vector3Stamped.h"
#include <sensor_msgs/NavSatFix.h>

double r;
double theta;
double count=0.0;
double wn;

//publisher
//geographic_msgs::GeoPoseStamped target_pose; //for global
geometry_msgs::PoseStamped target_pose; //for local
mavros_msgs::PositionTarget rotate_pose;

void state_cb(const mavros_msgs::State::ConstPtr msg){
    ROS_INFO("Connection Alived!");
    current_state = *msg;
}

void global_cb(const sensor_msgs::NavSatFix msg ){
	current_pose = msg;
	ROS_INFO("Current(lat,long,alt): %4.2f, %4.2f, %4.2f\n",msg.latitude, msg.longitude, msg.altitude);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "pub_setpoints");
    ros::NodeHandle n;

    //publisher
    ros::Publisher move_pub = n.advertise <geometry_msgs::PoseStamped> ("mavros/setpoint_position/local", 10);
    
    //subscriber
    ros::Subscriber state_sub = n.subscribe<mavros_msgs::State>("mavros/state", 1, state_cb);
    ros::Subscriber global_sub = n.subscribe<sensor_msgs::NavSatFix>("/mavros/global_position/global", 1, global_cb);
    
    //serviceClient
    ros::ServiceClient arming_client = n.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");
    ros::ServiceClient set_mode_client = n.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");

    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(20.0);
	
    //wn = rotating speed(maybe theta / sec?)
    //r = rotate diameter(may Meter?)
	nh.param("pub_setpoints_traj/wn", wn, 1.0);
	nh.param("pub_setpoints_traj/r", r, 1.0);
    
    
    // wait for FCU connection
    while(ros::ok() && !current_state.connected){
        ROS_INFO("Looped!");
        ros::spinOnce();
        rate.sleep();
    }

    target_pose.header.stamp = ros::Time::now();
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

    //send a few setpoints before starting
    for(int i = 100; ros::ok() && i > 0; --i){
        local_pos_pub.publish(target_pose);
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
            
            theta = wn*count*0.05;

    	    traget_pose.pose.position.x = r*sin(theta);
    	    traget_pose.pose.position.y = r*cos(theta);
    	    traget_pose.pose.position.z = 2;

	        count++;

		   move_pub.publish(target_pose);
        }
       ros::spinOnce();
       rate.sleep();

   }
