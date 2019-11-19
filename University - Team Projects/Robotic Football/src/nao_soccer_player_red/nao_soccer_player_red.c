/*
 * File:         nao_soccer_player_red.c
 * Description:  C controller for Webots Robocup Simulator
 * Author:       Michal Puheim, Technical University of Kosice
 */

#include "../nao_soccer_supervisor/RoboCupGameControlData.h"
#include <webots/camera.h>
#include <webots/servo.h>
#include <webots/accelerometer.h>
#include <webots/gyro.h>
#include <webots/touch_sensor.h>
#include <webots/robot.h>
#include <webots/utils/motion.h>
#include <webots/distance_sensor.h>
#include <webots/emitter.h>
#include <webots/receiver.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// team message header for inter robot communication
#define INFO_MESSAGE_STRUCT_HEADER "RTF_TUKE_PASSWORD"

// communication message structure definition
typedef struct  {  
  char header[sizeof(INFO_MESSAGE_STRUCT_HEADER) - 1]; // header to identify the structure (not null-termination)
  double ball_dist;                                    // send distance to ball info
  int attacking;                                       // send attaking token (robot who has it is attacking)
} InfoMessage;

// Communication message variable
InfoMessage G_message;

// RoboCup game control data structure
static struct RoboCupGameControlData gcd;

enum { GOALIE, PLAYER};
enum { RED, BLUE };

// this controller
static int player = -1;
static int color = -1;

// simulation time step
static int time_step;

// motion file handles
static WbMotionRef hand_wave, forwards, forwards50, backwards, side_step_left, side_step_right, turn_left_60, turn_right_60, turn_left_40, turn_right_40, turn_left_180, shoot;
static WbMotionRef current_motion, stand_up_from_front, stand_up_from_back;

// simulated devices
static WbDeviceTag camera, camera_select;              // camera with high/low position
static WbDeviceTag head_yaw, head_pitch;               // camera servos
static WbDeviceTag lshoulder_pitch, rshoulder_pitch;   // hand servos
static WbDeviceTag us_right_top, us_left_top;          // ultra sound top sensors
static WbDeviceTag us_right_bottom, us_left_bottom;    // ultra sound bottom sensors
static WbDeviceTag accelerometer, gyro;                // inertial unit
static WbDeviceTag fsr[2][4];                          // force sensitive resistors
static WbDeviceTag emitter, receiver;                  // inter-robot communication
static WbDeviceTag lfoot_lbumper, lfoot_rbumper;       // left foot bumpers
static WbDeviceTag rfoot_lbumper, rfoot_rbumper;       // right foot bumpers
static WbDeviceTag leds[7];                            // controllable led groups

// important constants
const double UNKNOWN = -999.9;
const double OFFSET_ANGLE = 0.6981;

// important global orientation variables
double G_ballCamDirectionAngle = -999.9;   // orientation towards camera (shows most current sensor information)
double G_ballCamElevationAngle = -999.9;
double G_ballCamDistance = -999.9;
double G_goalCamDirectionAngle = -999.9;
double G_goalCamElevationAngle = -999.9;
double G_goalCamDistance = -999.9;

double G_ballLastDirectionAngle = -999.9;  // orientation towards robot body (shows last valid sensor information - may be inaccurate)
double G_ballLastElevationAngle = -999.9;
double G_ballLastDistance = -999.9;
double G_goalLastDirectionAngle = -999.9;
double G_goalLastElevationAngle = -999.9;
double G_goalLastDistance = -999.9;

double G_LeftPoleDirection = -999.9;       // last location of goal poles
double G_LeftPoleElevation = -999.9;
double G_RightPoleDirection = -999.9;
double G_RightPoleElevation = -999.9;

// state variables
int STATE_getting_up = 0;
int STATE_attackking = 0;
int STATE_defending = 0;
int STATE_looking_at_goal = 0;
int STATE_looking_at_ball = 0;
int STATE_going_for_ball = 0;
int STATE_near_ball = 0;
int STATE_near_goal = 0;
int STATE_turned_to_goal = 0;
int STATE_kickoff = 0;

// global timers and counters
int G_info_timer = 5000;
int G_info_counter = 0;
int G_goal_look_timer = 5000;
int G_goal_look_counter = 0;
int G_kickoff_timer = 15000;
int G_kickoff_counter = 0;

// get team information
static void get_color_and_position() {
  const char *name = wb_robot_get_name();
  if (strstr(name, "red")) color = RED;
  if (strstr(name, "blue")) color = BLUE;
  if (strstr(name, "goal keeper")) player = GOALIE;
  if (strstr(name, "player")) player = PLAYER;
  if (color < 0 || player < 0) {
    //printf("Error: invalid robot name: %s", name);
    while (1) wb_robot_step(40);  // wait forever
  }
}

static void find_and_enable_devices() {    
  // camera
  camera = wb_robot_get_device("camera");
  wb_camera_enable(camera, time_step);

  // camera selection (high/low)
  camera_select = wb_robot_get_device("CameraSelect");
  
  // camera servos
  head_yaw = wb_robot_get_device("HeadYaw");
  head_pitch = wb_robot_get_device("HeadPitch");
  wb_servo_enable_position(head_yaw, time_step);     // enable returning position values - yaw and pitch
  wb_servo_enable_position(head_pitch, time_step);   // please notice that in this code orientation values has oposite sign than servo values
  wb_servo_set_position(camera_select, 1);           // use bottom camera
  wb_servo_set_position(head_pitch, 0.1 - OFFSET_ANGLE);  // set initial head position
  
  // hand servos
  lshoulder_pitch = wb_robot_get_device("LShoulderPitch");
  rshoulder_pitch = wb_robot_get_device("RShoulderPitch");

  // accelerometer
  accelerometer = wb_robot_get_device("accelerometer");
  wb_accelerometer_enable(accelerometer, time_step);

  // gyro
  gyro = wb_robot_get_device("gyro");
  wb_gyro_enable(gyro, time_step);
  
  // emitter & receiver
  emitter = wb_robot_get_device("emitter");
  receiver = wb_robot_get_device("receiver");
  wb_receiver_enable(receiver, time_step);

  // ultrasound sensors
  us_right_top = wb_robot_get_device("US/TopRight");
  us_right_bottom = wb_robot_get_device("US/BottomRight");
  us_left_top = wb_robot_get_device("US/TopLeft");
  us_left_bottom = wb_robot_get_device("US/BottomLeft");
  wb_distance_sensor_enable(us_right_top, time_step);
  wb_distance_sensor_enable(us_right_bottom, time_step);
  wb_distance_sensor_enable(us_left_top, time_step);
  wb_distance_sensor_enable(us_left_bottom, time_step);

  // foot sensors
  fsr[0][0] = wb_robot_get_device("LFsrFL");
  fsr[0][1] = wb_robot_get_device("LFsrFR");
  fsr[0][2] = wb_robot_get_device("LFsrBR");
  fsr[0][3] = wb_robot_get_device("LFsrBL");
  fsr[1][0] = wb_robot_get_device("RFsrFL");
  fsr[1][1] = wb_robot_get_device("RFsrFR");
  fsr[1][2] = wb_robot_get_device("RFsrBR");
  fsr[1][3] = wb_robot_get_device("RFsrBL");
  int i;
  for (i = 0; i < 4; i++) {
    wb_touch_sensor_enable(fsr[0][i], time_step);
    wb_touch_sensor_enable(fsr[1][i], time_step);
  }

  // foot bumpers
  lfoot_lbumper = wb_robot_get_device("LFoot/Bumper/Left");
  lfoot_rbumper = wb_robot_get_device("LFoot/Bumper/Right");
  rfoot_lbumper = wb_robot_get_device("RFoot/Bumper/Left");
  rfoot_rbumper = wb_robot_get_device("RFoot/Bumper/Left");
  wb_touch_sensor_enable(lfoot_lbumper, time_step);
  wb_touch_sensor_enable(lfoot_rbumper, time_step);
  wb_touch_sensor_enable(rfoot_lbumper, time_step);
  wb_touch_sensor_enable(rfoot_rbumper, time_step);

  // There are 7 controlable LED groups in Webots
  leds[0] = wb_robot_get_device("ChestBoard/Led");
  leds[1] = wb_robot_get_device("RFoot/Led");
  leds[2] = wb_robot_get_device("LFoot/Led");
  leds[3] = wb_robot_get_device("Face/Led/Right");
  leds[4] = wb_robot_get_device("Face/Led/Left");
  leds[5] = wb_robot_get_device("Ears/Led/Right");
  leds[6] = wb_robot_get_device("Ears/Led/Left"); 
}

// load motion files
static void load_motion_files() {  
  hand_wave = wbu_motion_new("HandWave.motion");
  forwards = wbu_motion_new("Forwards50c.motion");
  forwards50 = wbu_motion_new("Forwards50c.motion");
  backwards = wbu_motion_new("BackwardsB.motion");
  side_step_left = wbu_motion_new("SideStepLeftHalf.motion");
  side_step_right = wbu_motion_new("SideStepRightHalfB.motion");
  turn_left_60 = wbu_motion_new("TurnLeft60.motion");
  turn_right_60 = wbu_motion_new("TurnRight60.motion");
  turn_left_40 = wbu_motion_new("TurnLeft40.motion");
  turn_right_40 = wbu_motion_new("TurnRight40.motion");
  turn_left_180 = wbu_motion_new("TurnLeft180.motion");
  shoot = wbu_motion_new("ShootC.motion");
  stand_up_from_front = wbu_motion_new("StandUpFromFrontB.motion");
  stand_up_from_back = wbu_motion_new("StandUpFromBack.motion");
}

static void start_motion(WbMotionRef motion) {  
  // interrupt current motion
  if ((current_motion != motion) && (current_motion != NULL)) {
    wbu_motion_set_loop(current_motion, false);
    wbu_motion_stop(current_motion);
	wbu_motion_set_time(current_motion, 0); //set motion initial position to time 0
  }
  
  // start new motion
  wbu_motion_play(motion);
  wbu_motion_set_loop(motion, true);
  current_motion = motion;
}

static void stop_motion(WbMotionRef motion) {  
  if (motion != NULL) {
    wbu_motion_set_loop(motion, false);
    wbu_motion_stop(motion);
	wbu_motion_set_time(motion, 0); //set motion initial position to time 0
  }
  current_motion = NULL;
}

// normalize angle between -pi and +pi
static double normalizeAngle(double angle) {
  while (angle > 3.14159265) angle -= 2.0 * 3.14159265;
  while (angle < -3.14159265) angle += 2.0 * 3.14159265;
  return angle;
}

// compute floor distance between robot (feet) and ball
double getBallDistance(double cameraElevationAngle) {
  if (cameraElevationAngle == UNKNOWN)
    return UNKNOWN;
  double robot_height = 0.51; // approx robot camera base height with respect to ground in a standard posture of the robot
  double ball_radius = 0.043; // ball radius (size?)
  double cameraAngle = OFFSET_ANGLE;     // degrees between cameras axes - 0 for top camera, 0.6981 for bottom camera (only bottom is used currently)
  double ballElev = cameraElevationAngle - wb_servo_get_position(head_pitch) - cameraAngle;
  return ((robot_height - ball_radius) / tan(-ballElev));
}

// compute floor distance between robot (feet) and goal
double getGoalDistance(double cameraElevationAngle) {
  if (cameraElevationAngle == UNKNOWN)
    return UNKNOWN;
  double robot_height = 0.51; // approx robot camera base height with respect to ground in a standard posture of the robot
  double cameraAngle = OFFSET_ANGLE;     // degrees between cameras axes - 0 for top camera, 0.6981 for bottom camera (only bottom is used currently)
  double ballElev = cameraElevationAngle - wb_servo_get_position(head_pitch) - cameraAngle;
  return (robot_height / tan(-ballElev));
}

// compute floor direction between robot body (X - axis) and object  
double getAbsoluteDirAngle(double cameraDirectionAngle) {
  if (cameraDirectionAngle == UNKNOWN)
    return UNKNOWN;
  else
    return (cameraDirectionAngle - wb_servo_get_position(head_yaw)); // please notice that in this code orientation values has oposite sign than servo values
}

// compute elevation between default straight robot view and object  
double getAbsoluteEleAngle(double cameraElevationAngle) {
  if (cameraElevationAngle == UNKNOWN)
    return UNKNOWN;
  else
    return (cameraElevationAngle - wb_servo_get_position(head_pitch)); // please notice that in this code orientation values has oposite sign than servo values
}

// update long term orientation variables
static void update_orientation() {
  // compute aproximate absolute angle towards Ball
  if (G_ballCamDirectionAngle != UNKNOWN) {
    if (G_ballLastDirectionAngle != UNKNOWN)
	  G_ballLastDirectionAngle = normalizeAngle((2*G_ballLastDirectionAngle + getAbsoluteDirAngle(G_ballCamDirectionAngle))/3);
	else
	  G_ballLastDirectionAngle = normalizeAngle(getAbsoluteDirAngle(G_ballCamDirectionAngle));
  }
  // compute aproximate absolute angle towards Goal
  if (G_goalCamDirectionAngle != UNKNOWN) {
    if (G_goalLastDirectionAngle != UNKNOWN)
	  G_goalLastDirectionAngle = normalizeAngle((2*G_goalLastDirectionAngle + getAbsoluteDirAngle(G_goalCamDirectionAngle))/3);
	else
	  G_goalLastDirectionAngle = normalizeAngle(getAbsoluteDirAngle(G_goalCamDirectionAngle));
  }
  // compute aproximate distance to Ball and last elevation of Ball
  if (G_ballCamElevationAngle != UNKNOWN) {
    // elevation
    if (G_ballLastElevationAngle != UNKNOWN)
	  G_ballLastElevationAngle = normalizeAngle((2*G_ballLastElevationAngle + getAbsoluteEleAngle(G_ballCamElevationAngle))/3);
	else
	  G_ballLastElevationAngle = normalizeAngle(getAbsoluteEleAngle(G_ballCamElevationAngle));
	// distance
	if (G_ballLastDistance != UNKNOWN)
	  G_ballLastDistance = (2*G_ballLastDistance + getBallDistance(G_ballCamElevationAngle))/3;
	else
	  G_ballLastDistance = getBallDistance(G_ballCamElevationAngle);
  }
  // compute aproximate distance to Goal and last elevation of Goal
  if (G_goalCamElevationAngle != UNKNOWN) {
    // elevation
    if (G_goalLastElevationAngle != UNKNOWN)
	  G_goalLastElevationAngle = normalizeAngle((2*G_goalLastElevationAngle + getAbsoluteEleAngle(G_goalCamElevationAngle))/3);
	else
	  G_goalLastElevationAngle = normalizeAngle(getAbsoluteEleAngle(G_goalCamElevationAngle));
	// distance
	if (G_goalLastDistance != UNKNOWN)
	  G_goalLastDistance = (2*G_goalLastDistance + getGoalDistance(G_goalCamElevationAngle))/3;
	else
	  G_goalLastDistance = getGoalDistance(G_goalCamElevationAngle);
  }
}

// normalize rgb components to be less dependant of lighting
static void rgbnorm(const unsigned char in[3], double out[3]) {
  double max = in[0];
  if (in[1] > max) max = in[1];
  if (in[2] > max) max = in[2];
  out[0] = in[0] / max;
  out[1] = in[1] / max;
  out[2] = in[2] / max;
}

// find a blob whose rgb components match [R G B] with given tolerance
// return blob center as angles in radian with respect to camera axis
static void findColorBlob(const double ref[3], double tolerance, double *p_direction, double *p_elevation) {

  double direction;
  double elevation;

  double fov = wb_camera_get_fov(camera); //field of vision
  int width = wb_camera_get_width(camera);
  int height = wb_camera_get_height(camera);

  int length = width * height;  // number of pixels in the image
  int x = 0, y = 0;
  int npixels = 0;
  int i;
  
  // process latest image
  const unsigned char *p = wb_camera_get_image(camera);
  for (i = 0; i < length; i++, p+=3) {
    double pixel[3];
    rgbnorm(p, pixel);

    if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance) {
      x += i % width;
      y += i / width;
      npixels++;
    }
  }

  if (npixels > 2) {
    direction = ((double)x / npixels / width - 0.5) * fov;
    elevation = -((double)y / npixels / height - 0.5) * fov;
  }
  else {
    direction = UNKNOWN;
    elevation = UNKNOWN;
  }
  //printf("Found %d pixels from %d\n",npixels,length); // !!!!For debugging!!!!
  
  // return values
  *p_direction = direction;
  *p_elevation = elevation;
  
}

// prints camera image - blue goal and ball - (TODO - include yellow goal)
static void print_camera_image() {
  const double BLUE_GOAL_COLOR[3] = { 0.0, 0.8, 1.0 };
  const double YELLOW_GOAL_COLOR[3] = { 1.0, 1.0, 0.2 };

  const int SCALED = 2;

  int width = wb_camera_get_width(camera);
  int height = wb_camera_get_height(camera);

  // read rgb pixel values from the camera
  const unsigned char *p = wb_camera_get_image(camera);
  double pixel[3];
  
  printf("---------------camera image---------------\n");
  printf("original resolution: %d x %d, scaled to %d x %d\n",
    width, height, width / SCALED, height / SCALED);
  
  int y, x;
  char line[width / SCALED + 1];
  line[width / SCALED] = 0;  // add line termination
  for (y = 0; y < height; y += SCALED) {
    int count = 0;
    for (x = 0; x < width; x += SCALED, p += 3*SCALED) {
	  rgbnorm(p, pixel);
	  if (fabs(pixel[0] - BLUE_GOAL_COLOR[0]) < 0.3 && fabs(pixel[1] - BLUE_GOAL_COLOR[1]) < 0.3 && fabs(pixel[2] - BLUE_GOAL_COLOR[2]) < 0.3)
        line[count++] = 'X';
	  else if (fabs(pixel[0] - YELLOW_GOAL_COLOR[0]) < 0.3 && fabs(pixel[1] - YELLOW_GOAL_COLOR[1]) < 0.3 && fabs(pixel[2] - YELLOW_GOAL_COLOR[2]) < 0.3)
        line[count++] = 'X';
	  else if (fabs(pixel[0] - 1.0) < 0.2 && fabs(pixel[1] - 0.6) < 0.2 && fabs(pixel[2] - 0.2) < 0.2)
        line[count++] = 'O';	    
	  else
	    line[count++] = '.';
    }
	p += 3*width*(SCALED-1);
    line[count++] = 0;
    printf("%s\n", line);
  }
  printf("---------------camera image---------------\n");
}

// function which makes head servos move in order to have object on center of camera image
static void track_object(double objectDirectionAngle, double objectElevationAngle) {  
  wb_servo_set_position(head_yaw, -objectDirectionAngle);  // set servos to track object position
  wb_servo_set_position(head_pitch, -objectElevationAngle);
}

// print head servos position angle and robot position to ball and goal on console
static void show_info() {
  printf("\n");
  printf("Current Camera to Ball direction = %f\n",G_ballCamDirectionAngle);
  printf("Current Camera to Ball elevation = %f\n",G_ballCamElevationAngle);
  printf("Current Robot  to Ball direction = %f\n",getAbsoluteDirAngle(G_ballCamDirectionAngle));
  printf("Current Robot  to Ball distance  = %f\n",getBallDistance(G_ballCamElevationAngle));
  printf("\n");
  printf("Current Camera to Goal direction = %f\n",G_goalCamDirectionAngle);
  printf("Current Camera to Goal elevation = %f\n",G_goalCamElevationAngle);
  printf("Current Robot  to Goal direction = %f\n",getAbsoluteDirAngle(G_goalCamDirectionAngle));
  printf("Current Robot  to Goal distance  = %f\n",getGoalDistance(G_goalCamElevationAngle));
  printf("\n");
  printf("Head servo pitch = %f\n",wb_servo_get_position(head_pitch));
  printf("Head servo yaw   = %f\n",wb_servo_get_position(head_yaw));
  printf("\n");
  printf("Latest Robot to Ball elevation = %f\n",G_ballLastElevationAngle);
  printf("Latest Robot to Ball direction = %f\n",G_ballLastDirectionAngle);
  printf("Latest Robot to Ball distance  = %f\n",G_ballLastDistance);
  printf("\n");
  printf("Latest Robot to Goal elevation = %f\n",G_goalLastElevationAngle);
  printf("Latest Robot to Goal direction = %f\n",G_goalLastDirectionAngle);
  printf("Latest Robot to Goal distance  = %f\n",G_goalLastDistance);
  printf("\n");
}

// reset orientation (for example if robot falls down and is disoriented)
static void reset_orientation() {
  // reset global orientation variables
  G_ballCamDirectionAngle = -999.9;
  G_ballCamElevationAngle = -999.9;
  G_ballCamDistance = -999.9;
  G_goalCamDirectionAngle = -999.9;
  G_goalCamElevationAngle = -999.9;
  G_goalCamDistance = -999.9;
  G_ballLastDirectionAngle = -999.9;
  G_ballLastElevationAngle = -999.9;
  G_ballLastDistance = -999.9;
  G_goalLastDirectionAngle = -999.9;
  G_goalLastElevationAngle = -999.9;
  G_goalLastDistance = -999.9;
  G_LeftPoleDirection = -999.9;
  G_LeftPoleElevation = -999.9;
  G_RightPoleDirection = -999.9;
  G_RightPoleElevation = -999.9;

  // reset state variables
  STATE_getting_up = 0;
  STATE_attackking = 0;
  STATE_defending = 0;
  STATE_looking_at_goal = 0;
  STATE_looking_at_ball = 0;
  STATE_going_for_ball = 0;
  STATE_near_ball = 0;
  STATE_near_goal = 0;
  STATE_turned_to_goal = 0;

  // reset global time counters
  G_goal_look_counter = 0;
}

static void receive_data_packets() {

  G_message.ball_dist = UNKNOWN;
  G_message.attacking = -1;
  
  // read packets while the queue is not empty
  while (wb_receiver_get_queue_length(receiver) > 0) {
    
	//read head packet
	int packet_size = wb_receiver_get_data_size(receiver);
    const void *packet_bytes = wb_receiver_get_data(receiver);
	
	// verify packet type
	// Game control message
	if (packet_size == sizeof(struct RoboCupGameControlData) && memcmp(packet_bytes, GAMECONTROLLER_STRUCT_HEADER, 4) == 0) {
      // save packet bytes locally
      memcpy(&gcd, packet_bytes, packet_size);
    }
	// Team info message
	else if (packet_size == sizeof(InfoMessage) && memcmp(packet_bytes, INFO_MESSAGE_STRUCT_HEADER, 15) == 0) {
	  // save packet bytes locally
      memcpy(&G_message, packet_bytes, packet_size);
	}
	// Update actual STATES and another variables according to received message
	if ((G_message.ball_dist != UNKNOWN)&&(G_ballLastDistance != UNKNOWN)&&(G_message.ball_dist <= G_ballLastDistance)) {
	  STATE_attackking = 0;
	}
	if ((G_message.ball_dist != UNKNOWN)&&(G_ballLastDistance != UNKNOWN)&&(G_message.ball_dist > G_ballLastDistance)&&(G_message.attacking == 0)&&(player!=GOALIE)) {
	  STATE_attackking = 1;
	}
	
	//delete head packet and proceed to next packet
	wb_receiver_next_packet(receiver);
  }
}

static void send_data_packets() {
  InfoMessage info;
  memcpy(info.header, INFO_MESSAGE_STRUCT_HEADER, sizeof(INFO_MESSAGE_STRUCT_HEADER) - 1);
  info.ball_dist = G_ballLastDistance;
  info.attacking = STATE_attackking;
  wb_emitter_send(emitter, &info, sizeof(InfoMessage));
}

// function for getting up - declaration (for function run_simulation_step())
int getUpIfNecessary();

// run one or more simulations steps, increase time counters, receive info from transmitters, etc...
static void run_simulation_step(int count) {
  int i;
  for (i=0;i<count;i++)
  {   
	// increase timer counters
    if (G_info_counter <= G_info_timer)
	  G_info_counter += time_step;
	if (G_goal_look_counter <= G_goal_look_timer)
      G_goal_look_counter += time_step;
	if (G_kickoff_counter <= G_kickoff_timer)
      G_kickoff_counter += time_step;
	
	 // print robot position to ball and goal on console
	if (G_info_counter>G_info_timer){
	  //show_info();
   	  //print_camera_image();
	  G_info_counter=0;
    }
	
    if (STATE_getting_up == 0)
	  getUpIfNecessary();
    receive_data_packets();
	send_data_packets();
	
    if (wb_robot_step(time_step) == -1) {
      //reset_orientation();
	  //terminate(); //cleanup code here
	}
  }
}

// function for getting up - definition
int getUpIfNecessary() {
  const double *acc = wb_accelerometer_get_values(accelerometer);
  //printf("\nDEBUG accelerometer\nA1 = %f\nA2 = %f\nA3 = %f\n",acc[0],acc[1],acc[2]);
  
  //if robot is fallen on front
  if ((acc[0] < -5.0)&&(acc[2] < 3)) {
    //printf("\nDEBUG - Standing up from front\n");
	//set state changes
	reset_orientation();
    STATE_getting_up = 1;
	//stop current activity and stand up
	stop_motion(current_motion);
    wbu_motion_play(stand_up_from_front);
    do {
      run_simulation_step(1);
	  get_color_and_position(); // temp - check if team color has changed
    } while (! wbu_motion_is_over(stand_up_from_front));
	//set state changes
	STATE_getting_up = 0;
	STATE_looking_at_ball = 1;
    STATE_attackking = 1;
	G_goal_look_counter = 4800;
    return 1;
  }
  //if robot is fallen on back or side
  if (((acc[0] > 5.0)||(acc[1] > 5.0)||(acc[1] < -5.0))&&(acc[2] < 3)) {
    //printf("\nDEBUG - Standing up from back\n");
	//set state changes
	reset_orientation();
    STATE_getting_up = 1;
	//stop current activity and stand up
	stop_motion(current_motion);
    wbu_motion_play(stand_up_from_back);
    do {
      run_simulation_step(1);
	  get_color_and_position(); // temp - check if team color has changed
    } while (! wbu_motion_is_over(stand_up_from_back));
	//set state changes
    STATE_getting_up = 0;
	STATE_looking_at_ball = 1;
    STATE_attackking = 1;
	G_goal_look_counter = 4800;
    return 1;
  }
  STATE_getting_up = 0;
  return 0;
}

// find ball on camera picture
static void camera_update_ball_position() {
  const double BALL_COLOR[3] = { 1.0, 0.6, 0.2 }; // ball reference color
  findColorBlob(BALL_COLOR, 0.2, &G_ballCamDirectionAngle, &G_ballCamElevationAngle);
}

// scan area around robot to find ball
int ball_scan() {
  const int STEPS = 30;
  const double HEAD_YAW_INITIAL = 0;
  const double HEAD_YAW_MAX = 1.5708;    //maximal head turn to left
  const double HEAD_YAW_MIN = -1.5708;   //maximal head turn to right
  const double HEAD_YAW_RANGE = HEAD_YAW_MAX - HEAD_YAW_MIN;
  const double HEAD_PITCH_INITIAL = 0.1000 - OFFSET_ANGLE; // look position at upper part of FOV
  const double HEAD_PITCH_CENTER = 0.6500 - OFFSET_ANGLE;  // look position at middle part of FOV
  const double HEAD_PITCH_MIN = 0.7854;                    // look position at bottom part of FOV
  
  int i;
  double yawAngle;  
  
  // look if ball is in front of robot
  wb_servo_set_position(head_pitch, HEAD_PITCH_INITIAL);  // head straight position
  wb_servo_set_position(head_yaw, HEAD_YAW_INITIAL);
  run_simulation_step(3);                                 // progress 1 simulation step
  camera_update_ball_position();                          // look for ball on camera picture
  if (G_ballCamDirectionAngle != UNKNOWN)
	return 0; //ball found

  // scan right to left with horizontal head
  wb_servo_set_position(head_pitch, HEAD_PITCH_INITIAL);  // horizontal head
  wb_servo_set_position(head_yaw, HEAD_YAW_MIN);          // turn head right
  run_simulation_step(3);                                 // progress 3 simulation steps to give robot some time to turn head
  for (i = 0; i < STEPS; i++) {
    yawAngle = HEAD_YAW_MIN + ((double)i * HEAD_YAW_RANGE / STEPS);
	wb_servo_set_position(head_yaw, yawAngle);            // move head to new position
    run_simulation_step(1);                               // progress simulation step
    camera_update_ball_position();                        // look for ball on camera picture
    if (G_ballCamDirectionAngle != UNKNOWN)
	  return 0; //ball found
  }

  // scan left to right with lowered head
  wb_servo_set_position(head_pitch, HEAD_PITCH_CENTER);   // lower head
  wb_servo_set_position(head_yaw, HEAD_YAW_MAX);          // turn head left
  run_simulation_step(3);                                 // progress 3 simulation steps to give robot some time to turn head
  for (i = 0; i < STEPS; i++) {
    yawAngle = HEAD_YAW_MAX - ((double)i * HEAD_YAW_RANGE / STEPS);
	wb_servo_set_position(head_yaw, yawAngle);            // move head to new position
    run_simulation_step(1);                               // progress simulation step
    camera_update_ball_position();                        // look for ball on camera picture
    if (G_ballCamDirectionAngle != UNKNOWN)
	  return 0; //ball found
  }
  
  // scan right to left with even more lowered head
  wb_servo_set_position(head_pitch, HEAD_PITCH_MIN);      // lower head
  wb_servo_set_position(head_yaw, HEAD_YAW_MIN);          // turn head right
  run_simulation_step(3);                                 // progress 3 simulation steps to give robot some time to turn head
  for (i = 0; i < STEPS; i++) {
    yawAngle = HEAD_YAW_MIN + ((double)i * HEAD_YAW_RANGE / STEPS);
	wb_servo_set_position(head_yaw, yawAngle);            // move head to new position
    run_simulation_step(1);                               // progress simulation step
    camera_update_ball_position();                        // look for ball on camera picture
    if (G_ballCamDirectionAngle != UNKNOWN)
	  return 0; //ball found
  }

  // ball was not found
  return -1;
}

// find a center of goal whose rgb components match [R G B] with given tolerance
//      ______________________
//      |                    |
//      |                    |
//      |                    |
//      |                    |    x - goal center
//      |                    |
//      |         x          |
// if the whole goal is seen, then return goal coordinates
// if the goal is only partially seen by camera, return NULL for global goal orientation variables
static void far_goal_detect() {
  // reference colors
  const double BLUE_GOAL_COLOR[3] = { 0.0, 0.8, 1.0 };
  const double YELLOW_GOAL_COLOR[3] = { 1.0, 1.0, 0.2 };
  const double *ref;
  double tolerance = 0.3;

  if (color == RED)
    ref = BLUE_GOAL_COLOR;
  else
    ref = YELLOW_GOAL_COLOR;
  
  // poles orientation
  double LPdir = UNKNOWN;
  double LPele = UNKNOWN;
  double RPdir = UNKNOWN;
  double RPele = UNKNOWN;
  
  // goal orientation
  G_goalCamElevationAngle = UNKNOWN;
  G_goalCamDirectionAngle = UNKNOWN;

  double fov = wb_camera_get_fov(camera); //field of vision
  int width = wb_camera_get_width(camera);
  int height = wb_camera_get_height(camera);

  int length = width * height;  // number of pixels in the image
  double pixel[3];
  int npixels = 0;
  int i,j;
  
  // process latest image
  const unsigned char *picture = wb_camera_get_image(camera);
  const unsigned char *p = picture;
  
  // check if at least some pixels of goal are seen
  for (i = 0; i < length; i++, p+=3) {    
    rgbnorm(p, pixel);

    if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance) {
      npixels++;
    }
	if (npixels>20)
	  break; //found enough pixels to work with
  }
  if (npixels<=20) {
    // not enough pixels - goal not found
	//printf("DEBUG - few pixels\n");
    return;
  }

  // check if goal is seen partially by checking outer edges of image
  p = picture;
  int Lpixels = 0; //left pixels
  int Rpixels = 0; //right pixels
  int Tpixels = 0; //top pixels
  int Bpixels = 0; //bottom pixels
  for (i = 0; i < length; i++, p+=3) {
    // check left outer pixels
    if (i % width == 0) {
      rgbnorm(p, pixel);
      if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
        Lpixels++;
	}
	// check top outer pixels
	if (i / width == 0) {
      rgbnorm(p, pixel);
      if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
        Tpixels++;
	}
	// check right outer pixels
	if ((i % width == width-1)) {
      rgbnorm(p, pixel);
      if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
        Rpixels++;
	}
	// check bottom outer pixels
	if (i / width == height-1) {
      rgbnorm(p, pixel);
      if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
        Bpixels++;
	}
  }
  if ((Lpixels>1) || (Rpixels>1) || (Tpixels>1) ||(Bpixels>1)) {
    //printf("DEBUG - bad shot - L=%d, R=%d, T=%d, B=%d\n",Lpixels,Rpixels,Tpixels,Bpixels);
    return;  // found pixels on edges - whole goal not seen - cannot calculate goal center angle
  }
  //printf("DEBUG - good shot - L=%d, R=%d, T=%d, B=%d\n",Lpixels,Rpixels,Tpixels,Bpixels);
  
  // calculate goal center if the whole goal is seen
  int bottom_1_x = 0;  // coords of bottom pixel of the first pole
  int bottom_1_y = 0;
  int bottom_2_x = 0;  // coords of bottom pixel of the second pole
  int bottom_2_y = 0;
  int belongs_to_pole;
  const unsigned char *neighbour_pixel = picture; // pointer to neighbour pixel of searched one
  // find first pole bottom pixel
  for (i = 0; i < height; i++) {  // for each row in camera picture matrix
	p = picture + 3*i*width;      // set start pointer for this row
	for (j = 0; j < width; j++, p+=3) { // for each pixel in row
	  rgbnorm(p, pixel);
      if ((fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)&&(bottom_1_y < i)) {
	    // check if there are other pixels of such color around (for redundance)
	    belongs_to_pole = 0;
        neighbour_pixel = p-3;
		rgbnorm(neighbour_pixel, pixel); // check left pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p-3-3*width;
		rgbnorm(neighbour_pixel, pixel); // check top left pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p-3*width;
		rgbnorm(neighbour_pixel, pixel); // check top pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p+3-3*width;
		rgbnorm(neighbour_pixel, pixel); // check top rigth pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p+3;
		rgbnorm(neighbour_pixel, pixel); // check right pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p+3+3*width;
		rgbnorm(neighbour_pixel, pixel); // check bottom right pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p+3*width;
		rgbnorm(neighbour_pixel, pixel); // check bottom pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p-3+3*width;
		rgbnorm(neighbour_pixel, pixel); // check bottom left pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		// set pixel as the bottommost one in first pole
		if (belongs_to_pole > 5) {
		  bottom_1_x=j;
		  bottom_1_y=i;
		}
	  }
	}
  }
  //printf("DEBUG - Pole 1 - x = %d, y = %d\n",bottom_1_x,bottom_1_y);
  
  // find second pole bottom pixel
  for (i = 0; i < height; i++) {  // for each row in camera picture matrix
	p = picture + 3*width -3 + 3*i*width;      // set start pointer for this row
	for (j = width; j > 0; j--, p-=3) { // for each pixel in row
	  if ((fabs(j-bottom_1_x))<20)
	    continue; // skip neigborhood of first pole
	  rgbnorm(p, pixel);
      if ((fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)&&(bottom_2_y < i)) {
	    // check if there are other pixels of such color around (for redundance)
	    belongs_to_pole = 0;
        neighbour_pixel = p-3;
		rgbnorm(neighbour_pixel, pixel); // check left pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p-3-3*width;
		rgbnorm(neighbour_pixel, pixel); // check top left pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p-3*width;
		rgbnorm(neighbour_pixel, pixel); // check top pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p+3-3*width;
		rgbnorm(neighbour_pixel, pixel); // check top rigth pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p+3;
		rgbnorm(neighbour_pixel, pixel); // check right pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p+3+3*width;
		rgbnorm(neighbour_pixel, pixel); // check bottom right pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p+3*width;
		rgbnorm(neighbour_pixel, pixel); // check bottom pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		neighbour_pixel = p-3+3*width;
		rgbnorm(neighbour_pixel, pixel); // check bottom left pixel
		if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
		  belongs_to_pole++;
		// set pixel as the bottommost one in first pole
		if (belongs_to_pole > 5) {
		  bottom_2_x=j;
		  bottom_2_y=i;
		}
	  }
	}
  }
  //printf("DEBUG - Pole 2 - x = %d, y = %d\n",bottom_2_x,bottom_2_y);
   
  // calculate center as average of bottom pixels of poles
  int center_x,center_y; 
  center_x = (bottom_1_x + bottom_2_x)/2;
  center_y = (bottom_1_y + bottom_2_y)/2;
  if ((center_x != 0)&&(center_y != 0)){
    G_goalCamDirectionAngle = ((double)center_x / width - 0.5) * fov; //printf("DEBUG - direction\n");
    G_goalCamElevationAngle = -((double)center_y / height - 0.5) * fov; //printf("DEBUG - elevation\n");
	// calculate poles orientation
	if (bottom_1_x > bottom_2_x){
	  LPdir = ((double)bottom_2_x / width - 0.5) * fov; //printf("DEBUG - direction\n");
      LPele = -((double)bottom_2_y / height - 0.5) * fov; //printf("DEBUG - elevation\n");
	  RPdir = ((double)bottom_1_x / width - 0.5) * fov; //printf("DEBUG - direction\n");
      RPele = -((double)bottom_1_y / height - 0.5) * fov; //printf("DEBUG - elevation\n");
	}
	else {
	  LPdir = ((double)bottom_1_x / width - 0.5) * fov; //printf("DEBUG - direction\n");
      LPele = -((double)bottom_1_y / height - 0.5) * fov; //printf("DEBUG - elevation\n");
	  RPdir = ((double)bottom_2_x / width - 0.5) * fov; //printf("DEBUG - direction\n");
      RPele = -((double)bottom_2_y / height - 0.5) * fov; //printf("DEBUG - elevation\n");
	}
    G_LeftPoleDirection = getAbsoluteDirAngle(LPdir); //location of goal poles
    G_LeftPoleElevation = getAbsoluteEleAngle(LPele);
    G_RightPoleDirection = getAbsoluteDirAngle(RPdir);
    G_RightPoleElevation = getAbsoluteEleAngle(RPele);
  }
  //printf("DEBUG - Goal - x = %d, y = %d\n",center_x,center_y);
  return;
}

// finds aproximate goal center by looking at topleft corner of the goal
static void close_goal_detect() {
  // reference colors
  const double BLUE_GOAL_COLOR[3] = { 0.0, 0.8, 1.0 };
  const double YELLOW_GOAL_COLOR[3] = { 1.0, 1.0, 0.2 };
  const double *ref;
  double tolerance = 0.3;

  if (color == RED)
    ref = BLUE_GOAL_COLOR;
  else
    ref = YELLOW_GOAL_COLOR;
  
  // pole orientation
  double Pdir = UNKNOWN;
  
  // goal orientation
  G_goalCamElevationAngle = UNKNOWN;
  G_goalCamDirectionAngle = UNKNOWN;

  double fov = wb_camera_get_fov(camera); //field of vision
  int width = wb_camera_get_width(camera);
  int height = wb_camera_get_height(camera);

  int length = width * height;  // number of pixels in the image
  double pixel[3];
  int npixels = 0;
  int i;
  int x = 0;
  
  // process latest image
  const unsigned char *picture = wb_camera_get_image(camera);
  
  // check if goal is cut on left side (we are searching fot left pole at best)
  const unsigned char *p = picture;
  int Lpixels = 0; //left pixels
  for (i = 0; i < length; i++, p+=3) {
    // check left outer pixels
    if (i % width == 0) { //TODO - Optimize!!!
      rgbnorm(p, pixel);
      if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance)
        Lpixels++;
	}
  }
  if (Lpixels>2) {
    // image cut on edge - bad picture
    return;
  }
  
  // check the center row of image to find pole
  p = picture+3*(length/2);
  for (i = 0; i < width; i++, p+=3) {
    rgbnorm(p, pixel);
    if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance) {
      npixels++;
	  x += i;
	  continue;
    }
	if (npixels>5)
	  break;
  }
  
  // compute pole direction
  if (npixels<=8) {
    // not enough pixels - goal not found
	//printf("DEBUG - few pixels\n");
    return;
  }
  else {
    Pdir = ((double)x / npixels / width - 0.5) * fov;
  }
  
  // check if there are enough pixels of goal seen
  npixels = 0;
  p = picture;
  for (i = 0; i < length; i++, p+=3) { 
    rgbnorm(p, pixel);

    if (fabs(pixel[0] - ref[0]) < tolerance && fabs(pixel[1] - ref[1]) < tolerance && fabs(pixel[2] - ref[2]) < tolerance) {
      npixels++;
    }
	if (npixels>30)
	  break; //found enough pixels to work with
  }
  if (npixels<=30) {
    // not enough pixels - goal not found
	//printf("DEBUG - few pixels\n");
    return;
  }
  
  // find if goal is to left or right of the pole
  findColorBlob(ref, tolerance, &G_goalCamDirectionAngle, &G_goalCamElevationAngle);
  if (G_goalCamDirectionAngle == UNKNOWN) {    
    return;
  }
  else {
    //goal is on right side of the pole
    if (G_goalCamDirectionAngle > Pdir) {
	  G_goalCamDirectionAngle = Pdir + 0.2;
	  // aproximate direction of goal poles
	  G_LeftPoleDirection = getAbsoluteDirAngle(Pdir);
	  G_RightPoleDirection = getAbsoluteDirAngle(Pdir + 1.5 * fov);
	}
	//goal is on left side of the pole
	else {
	  G_goalCamDirectionAngle = Pdir - 0.2;
	  // aproximate direction of goal poles
	  G_RightPoleDirection = getAbsoluteDirAngle(Pdir);
	  G_LeftPoleDirection = getAbsoluteDirAngle(Pdir - 1.5 * fov);
	}
	  
	G_goalCamElevationAngle = -0.7; // aproximate elevation (with regard to the fact that goal is close)
	
	//aproximate elevation of goal poles
    G_LeftPoleElevation = getAbsoluteEleAngle(-0.7);
    G_RightPoleElevation = getAbsoluteEleAngle(-0.7);
  }
}

static void goal_detect() {
  far_goal_detect(); //find goal on camera picture using "far" approach
  if (G_goalCamDirectionAngle == UNKNOWN) {
	close_goal_detect(); //if goal is not found, try "close" approach for detection
	if (G_goalCamDirectionAngle != UNKNOWN)
	  STATE_near_goal = 1;
  }
  else
    STATE_near_goal = 0;
}

int goal_scan() {
  // important servo positions
  const int STEPS = 30;
  const double HEAD_YAW_INITIAL = 0;
  const double HEAD_YAW_MAX = 1.5708;    //maximal head turn to left
  const double HEAD_YAW_MIN = -1.5708;   //maximal head turn to right
  const double HEAD_YAW_RANGE = HEAD_YAW_MAX - HEAD_YAW_MIN;
  const double HEAD_PITCH_INITIAL = 0.0500 - OFFSET_ANGLE;
      
  int i;
  double yawAngle;
  
  // look if goal is in front of robot
  wb_servo_set_position(head_pitch, HEAD_PITCH_INITIAL);  // head straight position
  wb_servo_set_position(head_yaw, HEAD_YAW_INITIAL);
  run_simulation_step(2);                                 // progress 2 simulation steps
  goal_detect();
  if (G_goalCamDirectionAngle != UNKNOWN)
	return 0; //goal found
  
  // scan right to left with horizontal head
  wb_servo_set_position(head_pitch, HEAD_PITCH_INITIAL);  // horizontal head
  wb_servo_set_position(head_yaw, HEAD_YAW_MIN);          // turn head right
  run_simulation_step(3);                                 // progress 3 simulation steps to give robot some time to turn head
  for (i = 0; i < STEPS; i++) {
    yawAngle = HEAD_YAW_MIN + ((double)i * HEAD_YAW_RANGE / STEPS);
	wb_servo_set_position(head_yaw, yawAngle);            // move head to new position
    run_simulation_step(1);                               // progress simulation step
    goal_detect();
    if (G_goalCamDirectionAngle != UNKNOWN)	
	  return 0; //goal found
  }
  
  //goal not found
  return -1;
}

// main function for player
int player_main() {
  // set initial states
  STATE_getting_up = 0;
  STATE_attackking = 0;
  STATE_defending = 0;
  STATE_looking_at_goal = 0;
  STATE_looking_at_ball = 0;
  STATE_going_for_ball = 0;
  STATE_near_ball = 0;
  STATE_near_goal = 0;
  STATE_turned_to_goal = 0;
  STATE_kickoff = 1;
  
  // temporary variables
  int temp;
  
  // main control loop
  while (1) {
    // if game is in initial state then return to main (change team color and position accordingly)
    if ((int)gcd.state < 2) {
	  stop_motion(current_motion); // stop current motion if any
	  run_simulation_step(1); // run a simulation step
	  return 0;
	}
	
    // find ball but don't move while game is starting
    if ((int)gcd.state != 3) {
	  stop_motion(current_motion); // stop current motion if any
	  
	  if (G_ballCamDirectionAngle == UNKNOWN) // find ball if it is not found already
	    ball_scan();

      camera_update_ball_position(); //find ball on camera picture
	  update_orientation();  // update robot orientation variables
	  track_object(G_ballLastDirectionAngle,G_ballLastElevationAngle); // look at last known position of ball
	  
	  STATE_looking_at_ball = 1; // set initial state
      STATE_attackking = 1;
	  G_goal_look_counter = 4800;
	  G_kickoff_counter = 0;
	  
      run_simulation_step(1); // run a simulation step
	  continue;
	}
	
	// find if this team is on kick off
	if (STATE_kickoff == 1) {
	  if ((((int)gcd.kickOffTeam == 1)&&(color == RED))||(((int)gcd.kickOffTeam == 0)&&(color == BLUE)))
	    STATE_kickoff = 1;
	  if (G_kickoff_counter >= G_kickoff_timer)
	    STATE_kickoff = 0;
	}
	
	// run a simulation step
	camera_update_ball_position(); //find ball on camera picture
	goal_detect(); //find goal on camera picture
	update_orientation();  // update robot orientation variables
    run_simulation_step(1);
	
	// --MANAGE STATE CHANGES--
	// switch between looking at ball and goal
	if ((STATE_looking_at_ball == 1)&&(G_ballCamDirectionAngle != UNKNOWN)&&(G_goal_look_counter > G_goal_look_timer)&&(G_ballLastDistance != UNKNOWN)&&(G_ballLastDistance > 0.5)) {
	  G_goal_look_counter = 0;  //reset counter for timing the "look at goal" action
	  STATE_looking_at_ball = 0;
	  STATE_looking_at_goal = 1;
	}
	// if the ball is seen, move to it
	if ((STATE_near_ball == 0)&&(G_ballLastDirectionAngle != UNKNOWN))
	  STATE_going_for_ball = 1;
	// else do not move
	else if (STATE_near_ball == 0){
	  stop_motion(current_motion);  //stop current motion
	  STATE_going_for_ball = 0;
	}	
	
	// if the ball is far
	if (G_ballLastDistance > 0.5) {
	  STATE_going_for_ball = 1;
	  STATE_near_ball = 0;
	  STATE_turned_to_goal = 0;
	}
	
	// --MANAGE STATES--
	if (STATE_looking_at_ball == 1){
	  // if the ball is not seen on camera
	  if (G_ballCamDirectionAngle == UNKNOWN) {
	    // if there is knowledge of last ball position
	    if (G_ballLastDirectionAngle != UNKNOWN) {
	      track_object(G_ballLastDirectionAngle,G_ballLastElevationAngle); // look at last known position of ball
		  run_simulation_step(4);
		  camera_update_ball_position(); //find ball on camera picture
		  if (G_ballCamDirectionAngle == UNKNOWN) { // if the ball is still not seen on camera
		    G_ballLastDirectionAngle = UNKNOWN;
            G_ballLastElevationAngle = UNKNOWN;
            G_ballLastDistance = UNKNOWN;
		    continue;
		  }
		}
		// if there is NO knowledge of last ball position
		else if (STATE_near_ball == 0) {
		  while (ball_scan() == -1) {
			start_motion(turn_left_180); // turn to find ball
		  }
		  /* if (current_motion == turn_left_180)
			stop_motion(current_motion); */
		  camera_update_ball_position(); //find ball on camera picture
	      update_orientation();  // update robot orientation variables
		}
	  }
	  // if the ball is on camera
	  else {
	    track_object(G_ballLastDirectionAngle,G_ballLastElevationAngle); // look at last known position of ball
		camera_update_ball_position(); //find ball on camera picture
	  }	  
	}
	
	if (STATE_looking_at_goal == 1){
	  // if the goal is not seen on camera
	  if (G_goalCamDirectionAngle == UNKNOWN) {
	    // if there is knowledge of last goal position
	    if (G_goalLastDirectionAngle != UNKNOWN) {
		  if (STATE_near_goal == 0)
	        track_object(G_goalLastDirectionAngle,G_goalLastElevationAngle + 0.2); // look at last known position of goal
		  else
		    track_object(G_LeftPoleDirection - 0.15, 0.1 + OFFSET_ANGLE); // look at pole
		  run_simulation_step(6);
		  goal_detect(); //find goal on camera picture
		  if (G_goalCamDirectionAngle == UNKNOWN) { // if the goal is still not seen on camera
		    G_goalLastDirectionAngle = UNKNOWN;
            G_goalLastElevationAngle = UNKNOWN;
            G_goalLastDistance = UNKNOWN;
		    goal_scan();
		  }
		}
		// if there is NO knowledge of last goal position
		else
		  goal_scan();
	  }
	  // if the goal is on camera
	  else {
	    if (STATE_near_goal == 0)
	      track_object(G_goalLastDirectionAngle,G_goalLastElevationAngle + 0.2); // look at last known position of goal
		else {
		  track_object(G_LeftPoleDirection - 0.15, 0.1 + OFFSET_ANGLE); // look at pole
		}
		goal_detect(); //find goal on camera picture
	  }
	  if (STATE_near_ball == 0) {
	    STATE_looking_at_ball = 1;  // look at ball now
	    STATE_looking_at_goal = 0;
		G_goal_look_counter = 0;  //reset counter for timing the "look at goal" action
	  }
	}
	
	if (STATE_going_for_ball == 1) {
	  //turn and go to target
	  if (G_ballCamDirectionAngle != UNKNOWN) {
        if ((G_ballLastDirectionAngle < -0.4)&&(current_motion != turn_left_40)) {
		  stop_motion(current_motion);  //stop current motion
		  run_simulation_step(3);       //wait for robot to stabilize
	      start_motion(turn_left_40);   //start turn
		  G_goal_look_counter = 0;      //reset counter for timing the "look at goal" action - need to concentrate on ball for some time
		  // TODO (set goal poles dir angle change here)
		  if (G_goalLastDirectionAngle != UNKNOWN)
		    G_goalLastDirectionAngle -= G_ballLastDirectionAngle;
		}
	    else if ((G_ballLastDirectionAngle > 0.4)&&(current_motion != turn_right_40)) {
		  stop_motion(current_motion);  //stop current motion
		  run_simulation_step(3);       //wait for robot to stabilize
	      start_motion(turn_right_40);  //start turn
		  G_goal_look_counter = 0;      //reset counter for timing the "look at goal" action - need to concentrate on ball for some time
		  // TODO (set goal poles dir angle change here)
		  if (G_goalLastDirectionAngle != UNKNOWN)
		    G_goalLastDirectionAngle -= G_ballLastDirectionAngle;
		}
		else if ((G_ballLastDistance != UNKNOWN)&&(G_ballLastDistance < 1.0)&&(STATE_attackking == 0)) {
		   // if another robot is attacking, then go back from ball to make space for him
	       stop_motion(current_motion);  //stop current motion
		   run_simulation_step(3);       //wait for robot to stabilize
	       start_motion(backwards);  //start turn
	       //printf("\nDEBUG moving back to make space for another robot\n");
	    }
		else if ((G_ballLastDistance != UNKNOWN)&&(G_ballLastDistance < 1.2)&&(STATE_attackking == 0)) {
		   // if another robot is attacking, then wait at some distance from the ball
	       stop_motion(current_motion);  //stop current motion
	       //printf("\nDEBUG waiting for attack\n");
	    }
		else if ((G_ballLastDirectionAngle < 0.1)&&(G_ballLastDirectionAngle > -0.1)) {
		  if (current_motion != forwards) {
		    stop_motion(current_motion);  //stop current motion
		    run_simulation_step(6);       //wait for robot to stabilize
		  }
	      start_motion(forwards);       //go to ball
		}
		else if ((current_motion != turn_right_40)&&(current_motion != turn_left_40)) {
		  start_motion(forwards);       //go to ball
		}
	  }
	  if ((G_ballLastDistance != UNKNOWN)&&(G_ballLastDistance < 0.2)&&(STATE_kickoff == 0)) {
	    stop_motion(current_motion);  //stop current motion
		run_simulation_step(1);       //wait for robot to stabilize
		STATE_going_for_ball = 0;
		STATE_near_ball = 1;
	  }
	}
	
	if (STATE_near_ball == 1) {
	  //look at goal and turn to it
	  if (STATE_turned_to_goal == 0) {
	    // if robot is not in state of looking at goal, set it
	    if (STATE_looking_at_goal == 0) {
	      STATE_looking_at_goal = 1;
		  STATE_looking_at_ball = 0;
		}
		// if robot is in state of looking at goal, but he can't find it, start turning around
		else if (G_goalCamDirectionAngle == UNKNOWN) {
		  temp = 0;
		  while (goal_scan() == -1) {
			start_motion(turn_left_180);
			if (9 < temp++) {
			  start_motion(backwards); //if goal cannot be found, go back a little
			  STATE_near_ball = 0;
			  G_goalLastDirectionAngle = UNKNOWN;
              G_goalLastElevationAngle = UNKNOWN;
              G_goalLastDistance = UNKNOWN;
			  //printf ("DEBUG - cannot find goal - setting variables to UNKNOWN");
			  break;
			}
		  }
		  if (current_motion == turn_left_180) {
		    start_motion(forwards); // TEMP - stop and stabilize
			run_simulation_step(2);
			stop_motion(current_motion);
	      }
		}
		// if robot has goal on camera, turn to it
		else if (G_goalLastDirectionAngle != UNKNOWN) {
		  if ((G_goalLastDirectionAngle < -0.1)&&(current_motion != turn_left_40)) {
		    stop_motion(current_motion);  //stop current motion
		    run_simulation_step(4);       //wait for robot to stabilize
	        start_motion(turn_left_40);   //start turn
			STATE_looking_at_goal = 1;
		    STATE_looking_at_ball = 0;
			//printf("\nDEBUG turning LEFT - goal dir = %f ; goal_cam dir = %f\n",G_goalLastDirectionAngle, G_goalCamDirectionAngle);
		    // TODO (set goal poles dir angle change here)
		    if (G_goalLastDirectionAngle != UNKNOWN)
		      G_ballLastDirectionAngle -= G_goalLastDirectionAngle;
		  }
	      else if ((G_goalLastDirectionAngle > 0.1)&&(current_motion != turn_right_40)) {
		    stop_motion(current_motion);  //stop current motion
		    run_simulation_step(4);       //wait for robot to stabilize
	        start_motion(turn_right_40);  //start turn
		    STATE_looking_at_goal = 1;
		    STATE_looking_at_ball = 0;
			//printf("\nDEBUG turning RIGHT - goal dir = %f ; goal_cam dir = %f\n",G_goalLastDirectionAngle, G_goalCamDirectionAngle);
		    // TODO (set goal poles dir angle change here)
		    if (G_goalLastDirectionAngle != UNKNOWN)
		      G_ballLastDirectionAngle -= G_goalLastDirectionAngle;
		  }
	      else if ((G_goalLastDirectionAngle <= 0.1)&&(G_goalLastDirectionAngle >= -0.1)) {
		      stop_motion(current_motion);  //stop current motion
		      run_simulation_step(6);       //wait for robot to stabilize
			  STATE_turned_to_goal = 1;
			  STATE_looking_at_goal = 0;
			  STATE_looking_at_ball = 1;
			  //printf("\nDEBUG facing GOAL - goal dir = %f ; goal_cam dir = %f\n",G_goalLastDirectionAngle, G_goalCamDirectionAngle);
		  }
		}
	  }
	  //if turned to goal, step around ball for good shot position
	  else {
	    if (G_ballLastDirectionAngle == UNKNOWN) {
		  G_ballLastDistance = 0.2; //TEMP - set distance to some low value so other robots won't be coming close and making trouble
		  temp = 0;
		  while (ball_scan() == -1) {
			if (temp++ < 3) {
			  start_motion(backwards); // go back if the ball is not seen
			  //printf("\nDEBUG seraching for ball ...%d\n",temp);
			}
			else {
			  STATE_near_ball = 0; // if robot has not found ball while facing to goal, reset his state
			  STATE_turned_to_goal = 0;
			  stop_motion(current_motion);   //stop current motion
			  //printf("\nDEBUG ball not found\n");
			  break;
			}
		  }
		}
	    else if ((G_ballLastDirectionAngle < -0.35)&&(G_ballLastDirectionAngle > -0.78)&&(current_motion != side_step_left)) {
		  //printf("\nDEBUG step LEFT\n");
		  stop_motion(current_motion);   //stop current motion
		  run_simulation_step(4);        //wait for robot to stabilize
		  start_motion(side_step_left);  //step left		  
		}
		else if ((G_ballLastDirectionAngle > -0.15)&&(G_ballLastDirectionAngle < 0.78)&&(current_motion != side_step_right)) {
		  //printf("\nDEBUG step RIGHT\n");
		  stop_motion(current_motion);   //stop current motion
		  run_simulation_step(4);        //wait for robot to stabilize
		  start_motion(side_step_right); //step right
		}
	    else if (((G_ballLastDirectionAngle > 0.8)||(G_ballLastDirectionAngle < -0.8))&&(current_motion != backwards)) {
		  //printf("\nDEBUG go BACK\n");
		  stop_motion(current_motion);   //stop current motion
		  run_simulation_step(4);        //wait for robot to stabilize
		  start_motion(backwards);       //go back a little
		}
		else if ((G_ballLastDistance > 0.13)&&(G_ballLastDirectionAngle > -0.30)&&(G_ballLastDirectionAngle < -0.20)&&(current_motion != forwards)) {
		  //printf("\nDEBUG go FORWARD\n");
		  stop_motion(current_motion);   //stop current motion
		  run_simulation_step(4);        //wait for robot to stabilize
		  start_motion(forwards);      //move closer
		}
		else if ((G_ballLastDistance < 0.14)&&(G_ballLastDirectionAngle > -0.36)&&(G_ballLastDirectionAngle < -0.14)) {
		  //printf("\nDEBUG go SHOOT\n");
		  stop_motion(current_motion);   //stop current motion
		  start_motion(forwards);
		  run_simulation_step(2);        // TEMP - get initial posture for better balance
		  stop_motion(current_motion);
		  run_simulation_step(10);       //wait for robot to stabilize
		  start_motion(shoot);           //shoot
		  run_simulation_step(55);
		}
		else if (current_motion == NULL) {
		  //printf("\nDEBUG NULL motion\n");
		  stop_motion(current_motion);   //stop current motion
		  run_simulation_step(4);        //wait for robot to stabilize
		  start_motion(backwards);       //go back a little
		  run_simulation_step(4);
		}
	  }
	}
	//end of main cycle
  }
  return 0;
}

// goalie motion functions
int right(step_to_right){
  wbu_motion_play(side_step_right);
  do
    {
    run_simulation_step(1);
    }
    while(! wbu_motion_is_over(side_step_right));
  return step_to_right+1;
  }
  
int left(step_to_right){
  wbu_motion_play(side_step_left);
  do
    {
    run_simulation_step(1);
    }
    while(! wbu_motion_is_over(side_step_left));
  return step_to_right-1;
  }

// main function for goalie
int goalie_main() {
  // main control loop
  int i = 0;
  int step_to_right = 0;
  int step_to_front = 0;
  int side_step_max = 0;
  
  while (1)
  {
    // if game is in initial state then return to main (change team color and position accordingly)
    if ((int)gcd.state < 2) {
	  stop_motion(current_motion); // stop current motion if any
	  run_simulation_step(1); // run a simulation step
	  return 0;
	}
	
    if ((int)gcd.state != 3) {
	  stop_motion(current_motion); // stop current motion if any
	  
	  if (G_ballCamDirectionAngle == UNKNOWN) // find ball if it is not found already
	    ball_scan();

      camera_update_ball_position(); //find ball on camera picture
	  update_orientation();  // update robot orientation variables
	  track_object(G_ballLastDirectionAngle,G_ballLastElevationAngle); // look at last known position of ball
	  
	  //STATE_looking_at_ball = 1; // set initial state
    //STATE_attackking = 1;
	  G_goal_look_counter = 4800;
    
    //reset all values to default
    step_to_right = 0;
    step_to_front = 0;
    side_step_max = 0;
	  
      run_simulation_step(1); // run a simulation step
	  continue;
    }
  
    if (getUpIfNecessary() == 1)
      {
			//wbu_motion_play(turn_left_180);
      
        do
        {
        //if ((int)gcd.state != 3) break;
        if (getUpIfNecessary()==1) break;
        wbu_motion_play(turn_left_180);
        run_simulation_step(1);
        goal_scan();
        if (G_goalCamDirectionAngle != UNKNOWN) track_object(G_goalCamDirectionAngle,G_goalCamElevationAngle);
        //printf("%f",getAbsoluteDirAngle(G_goalCamDirectionAngle));
        }
        while ((getAbsoluteDirAngle(G_goalCamDirectionAngle) > 0.10) || (getAbsoluteDirAngle(G_goalCamDirectionAngle) < -0.10) );// wbu_motion_stop(turn_left_180);
        //}
        //while  (! wbu_motion_is_over(turn_left_180) );
        wbu_motion_stop(turn_left_180);
      }

    camera_update_ball_position(); //find ball on camera picture
    far_goal_detect(); //find goal on camera picture
    update_orientation();  // update robot orientation variables
    
    if (G_ballCamDirectionAngle == UNKNOWN)
    {
    ball_scan();
    }
    else
    {
    track_object(G_ballLastDirectionAngle,G_ballLastElevationAngle);
    //evaluate how much I can go from the middle of goal
    if ( getBallDistance(G_ballCamElevationAngle) < 0.5 ) 
      {
        side_step_max = 14;
      }
      else if ( getBallDistance(G_ballCamElevationAngle) < 1 ) side_step_max = 5;
      else if ( getBallDistance(G_ballCamElevationAngle) < 2 ) side_step_max = 3;
        else side_step_max = 0;
    }
    if (  ( getBallDistance(G_ballCamElevationAngle) < 0.7 ) && ( ( getAbsoluteDirAngle(G_ballCamDirectionAngle) > 1.3 ) || ( getAbsoluteDirAngle(G_ballCamDirectionAngle) < 1.3 ) )   )
      side_step_max = 13;
    if ( (G_ballCamDirectionAngle != UNKNOWN) )
      {
      //go to right side
      if ((getAbsoluteDirAngle(G_ballCamDirectionAngle) > 0.2) && (step_to_right < side_step_max))
        {        
        step_to_right=right(step_to_right);
        }
      //go to left side
      if ((getAbsoluteDirAngle(G_ballCamDirectionAngle) < -0.2) && (step_to_right > -side_step_max))
        {
        step_to_right=left(step_to_right);
        }

      //go to middle of goal, ball is far and in another direction
      if ( getBallDistance(G_ballCamElevationAngle) > 2 )
      {
        //if ( (getAbsoluteDirAngle(G_ballCamDirectionAngle) > 0) && (step_to_right < 0) ) step_to_right=right(step_to_right);
        //if ( (getAbsoluteDirAngle(G_ballCamDirectionAngle) < 0) && (step_to_right > 0) ) step_to_right=left(step_to_right);
        while (step_to_right > 0) step_to_right=left(step_to_right);
        while (step_to_right < 0) step_to_right=right(step_to_right);
      }
      
      //go kick ball, it should be in range
      if ((getAbsoluteDirAngle(G_ballCamDirectionAngle) < 0.35) && (getAbsoluteDirAngle(G_ballCamDirectionAngle) > -0.35) && (getBallDistance(G_ballCamElevationAngle) < 0.40) && (step_to_front < 1))
        {
        for (i = 0; i < 5; i++) 
          {
          wbu_motion_play(forwards);
          do
            {
            run_simulation_step(1);
            }
            while(! wbu_motion_is_over(forwards));
          step_to_front += 1;
          }
        }

      //go back to goal after kicking ball
      if ((getBallDistance(G_ballCamElevationAngle) > (0.4-step_to_front*0.1)) && (step_to_front > 0))
        {
        wbu_motion_play(backwards);
        do
          {
          run_simulation_step(1);
          }
          while(! wbu_motion_is_over(backwards));
        step_to_front -= 1;
        }
      }
      else //robot don't know where is ball -> go to middle of goal
      {
        if (step_to_front > 0)
          {
          wbu_motion_play(backwards);
          do
            {
            run_simulation_step(1);
            }
            while(! wbu_motion_is_over(backwards));
          step_to_front -= 1;
          }
        if (step_to_right > 0) step_to_right=left(step_to_right);
        if (step_to_right < 0) step_to_right=right(step_to_right);
      }
    run_simulation_step(1);
  }
  return 0;
}

int main() {
  // initialize
  wb_robot_init();
  
  // set basic time step
  time_step = wb_robot_get_basic_time_step();

  // enable sensors
  find_and_enable_devices();  
    
  // load motions
  load_motion_files();
  
  while(1) {
    // set initial motion to none
    current_motion = NULL;
  
    // set initial posture position
    wbu_motion_play(shoot);
    wb_robot_step(time_step);
    wbu_motion_stop(shoot);
    wbu_motion_set_time(shoot, 0);
    
	// get team information
	get_color_and_position();
    if (player == GOALIE)
      goalie_main();  // run goalie script
    else if (player == PLAYER)
      player_main();  // run player script
    else
	  wb_robot_step(time_step); // wait forever
  }
  return 0;
}