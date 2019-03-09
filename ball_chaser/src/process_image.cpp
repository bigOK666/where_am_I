#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // DONE TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Drive the robot!");
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    if(!client.call(srv))
	ROS_ERROR("Fail to call service ball_chaser command_robot");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    float pixel_position_divide = 0.0;
    float lin_x;
    float ang_z;
    bool white_ball = false;
    // DONE TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    for (int i=0; i < img.height * img.width; i++) {
	if ((img.data[i*3] + img.data[i*3+1] + img.data[i*3+2]) == white_pixel*3) {// only when three chanels are white, the sum of three chanels is 255*3
		white_ball = true;
	    	pixel_position_divide = (float)(i%img.width)/img.width;
		ROS_INFO_STREAM("i is: " + std::to_string(i) + " img.data[i] is: " + std::to_string(img.data[i]) + " position is: " + std::to_string(pixel_position_divide));
		break;    
	}  
    }  
    if (pixel_position_divide < 0.333) {// turn left if the ball is at left side
	    lin_x = 0.1;
	    ang_z = 0.1;
    }
    else if(pixel_position_divide >= 0.333 && pixel_position_divide < 0.667){// go straight
	    lin_x = 0.1;
	    ang_z = 0.0;
    }
    else{// turn right if the ball is at right side
	    lin_x = 0.1;
	    ang_z = -0.1;
    }
    if ( white_ball ){
	drive_robot(lin_x, ang_z);
    }
    else{
	drive_robot(0.0, 0.0);
    }
    
    
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
