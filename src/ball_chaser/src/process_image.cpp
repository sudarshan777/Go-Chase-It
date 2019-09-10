#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        
            ROS_ERROR("failed to call service drive_bot");
        
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int height ;
	int track;
		//ROS_INFO("DriveToTargetRequest received - linear_x:%1.2f, angular_z:%1.2f",(float)req.linear_x,(float)req.angular_z);
	bool white_pixel_present = false;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    
    // TODO: Loop through each pixel in the image and check if there's a bright white one
	for(int i = 0 ; i < img.height * img.step; i += 3)
	{
      if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel)
	{
        height = i/img.step;
        white_pixel_present = true;
        track = i;
        break;
      }
	}
  
  	
       
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    
	int lower_bound = height*img.step;
	int first_inter_bound = height*img.step + img.step/3;
	int second_inter_bound = height*img.step + 2*img.step/3;
	int upper_bound=(height+1)*img.step;
    if (white_pixel_present == false) // stop robot
            {
               drive_robot(0.0,0.0); 
            }
	else if (track >= lower_bound && track < first_inter_bound  && white_pixel_present == true )  // left side of the image
			{
				drive_robot(0.1, 0.5);
			}
	else if (track >= first_inter_bound && track < second_inter_bound && white_pixel_present == true)  // middle of the image
			{
				drive_robot(0.9,0.0); 
			}
	else   // right side of the image 
			{
				drive_robot(0.1,-0.5);
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