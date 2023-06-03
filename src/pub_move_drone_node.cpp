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
#include <sensor_msgs/LaserScan.h>

int step = 0;
double average = 0;
int cnt_clk=0;

//publisher
//geographic_msgs::GeoPoseStamped target_pose; //for global
geometry_msgs::PoseStamped target_pose; //for local


//subscriber
mavros_msgs::State current_state;
geometry_msgs::Point current_pose;
geometry_msgs::Point QR_loc;
geometry_msgs::Point server_data;
sensor_msgs::LaserScan lidar_scan;

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

// void global_cb(const sensor_msgs::NavSatFix msg ){
// 	current_pose = msg;
// 	ROS_INFO("Current(lat,long,alt): %4.2f, %4.2f, %4.2f\n",msg.latitude, msg.longitude, msg.altitude);
// }


void recv_laser(const sensor_msgs::LaserScan msg){
    lidar_scan = msg;
    double sum = 0.0;
    int count = 0;
    for (int i=30; i<91; ++i)
    {
        sum += lidar_scan.ranges[i];
        ++count;
    }
    average = sum / count;

    ROS_INFO("%f",average);

    //ROS_INFO("Current(angle_min,max,increment / range_min, max): %d, %d, %d, %d, %d", lidar_scan.angle_min, lidar_scan.angle_max, lidar_scan.angle_increment, lidar_scan.range_min, lidar_scan.range_max);
}

void state_cb(const mavros_msgs::State::ConstPtr msg){
    current_state = *msg;
} 

void local_cb(const geometry_msgs::PoseStamped msg ){
	current_pose = msg.pose.position;
	ROS_INFO("Current(x,y,z): %4.2f, %4.2f, %4.2f\n",current_pose.x, current_pose.y, current_pose.z);
}

void QR_cb(const geometry_msgs::PoseStamped msg ){
	QR_loc = msg.pose.position;
	ROS_INFO("QR loc(x,y): %4.2f, %4.2f\n", QR_loc.x, QR_loc.y);
}

void server_cb(const geometry_msgs::PoseStamped msg ){
	server_data = msg.pose.position;
	ROS_INFO("HOME : %4.2f, Next : %4.2f, BIRD : %4.2f\n",server_data.x, server_data.y, server_data.z);
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
   
   //ros::Subscriber target_alt_sub = n.subscribe<geographic_msgs::GeoPoseStamped>("targeting_alt",1,target_alt_cb);
   //ros::Subscriber target_sub = n.subscribe<geometry_msgs::PoseStamped>("targeting",10,target_cb);
   //ros::Subscriber global_sub = n.subscribe<sensor_msgs::NavSatFix>("/mavros/global_position/global", 1, global_cb);

   ros::Subscriber state_sub = n.subscribe<mavros_msgs::State>("mavros/state", 1, state_cb);
   ros::Subscriber lidar_sub = n.subscribe<sensor_msgs::LaserScan>("/laser/scan",1, recv_laser);
   ros::Subscriber QR_loc_sub = n.subscribe<geometry_msgs::PoseStamped>("/server/QR_locating",1,QR_cb);
   ros::Subscriber server_data_sub = n.subscribe<geometry_msgs::PoseStamped>("/server/data",1,server_cb);
   ros::Subscriber local_sub = n.subscribe<geometry_msgs::PoseStamped>("/mavros/local_position/pose", 1, local_cb);

   //servicesClient
   ros::ServiceClient arming_client = n.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");
   ros::ServiceClient set_mode_client = n.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");

   ros::Rate rate(2);

    while(ros::ok() && !current_state.connected){
        ROS_INFO("Trying to Connect...");
        ros::spinOnce();
        rate.sleep();
    }
    target_pose.header.stamp = ros::Time::now();
    //target_pose.header.frame_id = 1;
    target_pose.pose.position.x = 0;
    target_pose.pose.position.y = 0;
    target_pose.pose.position.z = 2;

//    lidar_scan.angle_max = 5;
//    lidar_scan.angle_min = -5;
//    lidar_scan.range_max = 10;
//    lidar_scan.range_min = 0.5;

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
        
       ROS_INFO("Target(x,y,z): %4.2f, %4.2f, %4.2f\n",target_pose.pose.position.x, target_pose.pose.position.y, target_pose.pose.position.z);
       target_pose.header.stamp = ros::Time::now();
       target_pose.header.frame_id = 1;
       
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
        
        // 101 102 103
        // 201 202 203
        //     drone
        // height of  1 floor = 2.5
        


        //201 --> x 5 y 6
        //201 - (4,1) --> x 1 y 4 z 1.5
        
        //takeoff
        if(step==0){
            target_pose.pose.position.x = 1;
            target_pose.pose.position.y = 4;
            target_pose.pose.position.z = 2.1;
            move_pub.publish(target_pose);
            if(current_pose.z>=1.8){step=1;}
        }
        
        //go upside
        if(step==1){
            if(average>4)
            {
                target_pose.pose.position.x = current_pose.x + 0.5;
            }
            else if(average<1)
            {
                target_pose.pose.position.x = current_pose.x - 0.5;
            }
            target_pose.pose.position.y = 4;
            target_pose.pose.position.z = current_pose.z + 1.0;
            move_pub.publish(target_pose);

            //QR detected
            if(QR_loc.x && QR_loc.y){
                ROS_INFO("HOME info : %4.2f,  BIRD Detected: %4.2f\n",server_data.x, server_data.z);
            }
            if(current_pose.z>=8.5){step=2;}
        }

        //go rightsie
        if(step ==2){
            if(average>4)
            {
                target_pose.pose.position.x = current_pose.x + 0.5;
            }
            else if(average<1)
            {
                target_pose.pose.position.x = current_pose.x - 0.5;
            }
            target_pose.pose.position.y = current_pose.y-1.0;
            target_pose.pose.position.z =8.5;
            move_pub.publish(target_pose);
            //QR detection func
             if(QR_loc.x && QR_loc.y){
                ROS_INFO("HOME info : %4.2f,  BIRD Detected: %4.2f\n",server_data.x, server_data.z);
            }

            if(current_pose.y<=1.7){step=3;}
        }

        //go downside
        if(step == 3){
            if(average>4)
            {
                target_pose.pose.position.x = current_pose.x + 0.5;
            }
            else if(average<1)
            {
                target_pose.pose.position.x = current_pose.x - 0.5;
            }
            target_pose.pose.position.y = 2;
            target_pose.pose.position.z = current_pose.z - 1.0;
            move_pub.publish(target_pose);
            //QR detection func
             if(QR_loc.x && QR_loc.y){
                ROS_INFO("HOME info : %4.2f,  BIRD Detected: %4.2f\n",server_data.x, server_data.z);
            }
            
            if(current_pose.z<= 1.5){step=4;}
        }
        
        //land on
        if(step==4)
        {   
            if(average>4)
            {
                target_pose.pose.position.x = current_pose.x + 0.5;
            }
            else if(average<1)
            {
                target_pose.pose.position.x = current_pose.x - 0.5;
            }
            target_pose.pose.position.y = -8;
            target_pose.pose.position.z = 0;
            move_pub.publish(target_pose);           
        }

        //QR 검출 -> 동과 호수 => 401이면 옆으로가기, 402면 아래로 가기 시작
        //

        //201 - (1,1) --> x 1 y 4 z 10.5
        // target_pose.pose.position.x = 1;
        // target_pose.pose.position.y = 4;
        // target_pose.pose.position.z = 10.5;
        // move_pub.publish(target_pose);
        

        
        //     ROS_INFO("CLK: %d\n",clk_count);
        //     clk_count++;
        //     if(!(clk_count%2)) {
        //         ROS_INFO("MOVE\n");
        //         time_count++;
        //         target_pose.pose.position.x = r*sin(wn*time_count);
        //         target_pose.pose.position.y = r-r*cos(wn*time_count);
        //         target_pose.pose.position.z = 2;
        //     }
        //    move_pub.publish(target_pose);
        //    time_count%=120;
            //rotate_pub.publish(rotate_pose);
            //alt_pub.publish(alt_pose);

        }

       ros::spinOnce();
       rate.sleep();

   }
   return 0;
}
