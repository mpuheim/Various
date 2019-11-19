//---------------------------------------------------------------------------------
//  File:         nao_soccer_player_blue.c
//  Description:  Sample C controller program for the Robotstadium contest.
//                This can be used as starting example to implement your controller
//  Project:      Robotstadium, the online robot soccer competition
//  Authors:      Olivier Michel & Yvan Bourquin, Cyberbotics Ltd.
//  Changes:      November 6, 2008: Adapted to Webots 6
//                February 13, 2009: Adapted to NaoV3R
//---------------------------------------------------------------------------------

#include "../nao_soccer_supervisor/RoboCupGameControlData.h"
#include <webots/utils/motion.h>
#include <webots/robot.h>
#include <webots/servo.h> 
#include <webots/camera.h>
#include <webots/distance_sensor.h>
#include <webots/accelerometer.h>
#include <webots/gyro.h>
#include <webots/touch_sensor.h>
#include <webots/led.h>
#include <webots/emitter.h>
#include <webots/receiver.h>
#include <string.h>
#include <stdio.h>

#define TIME_STEP 40

enum { GOALIE, PLAYER1, PLAYER2 };
enum { RED, BLUE };

// this controller
static int player = -1;
static int color = -1;

// initial selection
static int selected_player = PLAYER1;

// simulated devices
static WbDeviceTag camera, camera_select;             // camera with high/low position
static WbDeviceTag us[4];                             // ultra sound sensors
static WbDeviceTag accelerometer, gyro;               // inertial unit
static WbDeviceTag fsr[2][4];                         // force sensitive resistors
static WbDeviceTag emitter, super_emitter, receiver;  // inter-robot communication
static WbDeviceTag lfoot_lbumper, lfoot_rbumper;      // left foot bumpers
static WbDeviceTag rfoot_lbumper, rfoot_rbumper;      // right foot bumpers
static WbDeviceTag leds[7];                           // controllable led groups

// motion file handles
static WbMotionRef hand_wave, forwards, backwards, side_step_left, side_step_right, turn_left_60, turn_right_60;
static WbMotionRef currently_playing = NULL;

// RoboCup game control data
static struct RoboCupGameControlData gcd;

static const char *my_color_string() {
  switch (color) {
  case RED:  return "RED";
  case BLUE: return "BLUE";
  default:   return NULL;
  }
}

static void find_and_enable_devices() {
  const char *name = wb_robot_get_name();
  if (strstr(name, "red")) color = RED;
  if (strstr(name, "blue")) color = BLUE;
  if (strstr(name, "goal keeper")) player = GOALIE;
  if (strstr(name, "player 1")) player = PLAYER1;
  if (strstr(name, "player 2")) player = PLAYER2;
  if (color < 0 || player < 0) {
    printf("Error: invalid robot name: %s", name);
    while (1) wb_robot_step(TIME_STEP);  // wait forever
  }

  // camera
  camera = wb_robot_get_device("camera");
  wb_camera_enable(camera, 4 * TIME_STEP);

  // camera selection (high/low)
  camera_select = wb_robot_get_device("CameraSelect");

  // accelerometer
  accelerometer = wb_robot_get_device("accelerometer");
  wb_accelerometer_enable(accelerometer, TIME_STEP);

  // gyro
  gyro = wb_robot_get_device("gyro");
  wb_gyro_enable(gyro, TIME_STEP);

  // ultrasound sensors
  us[0] = wb_robot_get_device("US/TopRight");
  us[1] = wb_robot_get_device("US/BottomRight");
  us[2] = wb_robot_get_device("US/TopLeft");
  us[3] = wb_robot_get_device("US/BottomLeft");
  int i;
  for (i = 0; i < 4; i++)
    wb_distance_sensor_enable(us[i], TIME_STEP);

  // foot sensors
  fsr[0][0] = wb_robot_get_device("LFsrFL");
  fsr[0][1] = wb_robot_get_device("LFsrFR");
  fsr[0][2] = wb_robot_get_device("LFsrBR");
  fsr[0][3] = wb_robot_get_device("LFsrBL");
  fsr[1][0] = wb_robot_get_device("RFsrFL");
  fsr[1][1] = wb_robot_get_device("RFsrFR");
  fsr[1][2] = wb_robot_get_device("RFsrBR");
  fsr[1][3] = wb_robot_get_device("RFsrBL");
  for (i = 0; i < 4; i++) {
    wb_touch_sensor_enable(fsr[0][i], TIME_STEP);
    wb_touch_sensor_enable(fsr[1][i], TIME_STEP);
  }

  // foot bumpers
  lfoot_lbumper = wb_robot_get_device("LFoot/Bumper/Left");
  lfoot_rbumper = wb_robot_get_device("LFoot/Bumper/Right");
  rfoot_lbumper = wb_robot_get_device("RFoot/Bumper/Left");
  rfoot_rbumper = wb_robot_get_device("RFoot/Bumper/Left");
  wb_touch_sensor_enable(lfoot_lbumper, TIME_STEP);
  wb_touch_sensor_enable(lfoot_rbumper, TIME_STEP);
  wb_touch_sensor_enable(rfoot_lbumper, TIME_STEP);
  wb_touch_sensor_enable(rfoot_rbumper, TIME_STEP);

  // There are 7 controlable LED groups in Webots
  leds[0] = wb_robot_get_device("ChestBoard/Led");
  leds[1] = wb_robot_get_device("RFoot/Led");
  leds[2] = wb_robot_get_device("LFoot/Led");
  leds[3] = wb_robot_get_device("Face/Led/Right");
  leds[4] = wb_robot_get_device("Face/Led/Left");
  leds[5] = wb_robot_get_device("Ears/Led/Right");
  leds[6] = wb_robot_get_device("Ears/Led/Left");

  // emitter & receiver
  emitter = wb_robot_get_device("emitter");
  receiver = wb_robot_get_device("receiver");
  wb_receiver_enable(receiver, TIME_STEP);

  // for sending 'move' request to Supervisor
  super_emitter =  wb_robot_get_device("super_emitter");

  // keyboard
  wb_robot_keyboard_enable(10 * TIME_STEP); 
}

// load motion files
static void load_motion_files() {
  hand_wave = wbu_motion_new("../motions/HandWave.motion");
  forwards = wbu_motion_new("../motions/Forwards.motion");
  backwards = wbu_motion_new("../motions/Backwards.motion");
  side_step_left = wbu_motion_new("../motions/SideStepLeft.motion");
  side_step_right = wbu_motion_new("../motions/SideStepRight.motion");
  turn_left_60 = wbu_motion_new("../motions/TurnLeft60.motion");
  turn_right_60 = wbu_motion_new("../motions/TurnRight60.motion");
}

static void start_motion(WbMotionRef motion) {
  
  // interrupt current motion
  if (currently_playing)
    wbu_motion_stop(currently_playing);
  
  // start new motion
  wbu_motion_play(motion);
  currently_playing = motion;
}

static void receive_data_packets() {
  // if the queue is empty: nothing to print
  if (wb_receiver_get_queue_length(receiver) == 0)
    return;

  // keep only the most recent packet
  while (wb_receiver_get_queue_length(receiver) > 1)
    wb_receiver_next_packet(receiver);
  
  // verify packet type
  int packet_size = wb_receiver_get_data_size(receiver);
  const void *packet_bytes = wb_receiver_get_data(receiver);
  if (packet_size == sizeof(struct RoboCupGameControlData) && memcmp(packet_bytes, GAMECONTROLLER_STRUCT_HEADER, 4) == 0) {
    // save packet bytes locally
    memcpy(&gcd, packet_bytes, packet_size);
    return;
  }

  // handle other types of packets here ...
  // for example inter-robot communication, etc.
  printf("received unknown data packet of size: %d", packet_size);
}

// information that describes a team
static void print_team_info(const struct TeamInfo *team) {
  printf("  teamColour: %d\n", team->teamColour);
  printf("  score: %d\n", team->score);
}

// robocup game control date
static void print_game_control_data() {
  printf("----------RoboCupGameControlData----------\n");
  printf("playersPerTeam: %d\n", (int)gcd.playersPerTeam);
  printf("state: %d\n", (int)gcd.state);
  printf("firstHalf: %d\n", (int)gcd.firstHalf);
  printf("kickOffTeam: %d\n", (int)gcd.kickOffTeam);
  printf("secsRemaining: %u\n", gcd.secsRemaining);
  printf("teams[TEAM_BLUE]:\n");
  print_team_info(&gcd.teams[TEAM_BLUE]);
  printf("teams[TEAM_RED]:\n");
  print_team_info(&gcd.teams[TEAM_RED]);
  // For training only: this will be 0.0 during Robotstadium contest rounds:
  printf("ballXPos: %.3f\n", gcd.ballXPos);
  printf("ballZPos: %.3f\n", gcd.ballZPos);
  printf("----------RoboCupGameControlData----------\n");
}

// the accelerometer axes are oriented as on the real robot
// however the sign of the returned values may be opposite
static void print_acceleration() {
  const double *acc = wb_accelerometer_get_values(accelerometer);
  printf("----------accelerometer----------\n");
  printf("acceleration: [ x y z ] = [%f %f %f]\n", acc[0], acc[1], acc[2]);
  printf("----------accelerometer----------\n");
}

// the gyro axes are oriented as on the real robot
// however the sign of the returned values may be opposite
static void print_gyro() {
  const double *vel = wb_gyro_get_values(gyro);
  printf("----------gyro----------\n");
  printf("angular velocity: [ x y ] = [%f %f]\n", vel[0], vel[1]);
  printf("----------gyro----------\n");
}

static void print_foot_sensors() {
  double newtons = 0.0;
  double fsv[2][4]; // force sensor values
  int i;
  for (i = 0; i < 4; i++) {
    fsv[0][i] = wb_touch_sensor_get_value(fsr[0][i]);
    fsv[1][i] = wb_touch_sensor_get_value(fsr[1][i]);
    newtons += fsv[0][i] + fsv[1][i];
  }
  
  printf("----------foot sensors----------\n");
  printf("   left       right\n");
  printf("+--------+ +--------+\n");
  printf("|%3.1f  %3.1f| |%3.1f  %3.1f|  front\n", fsv[0][0], fsv[0][1], fsv[1][0], fsv[1][1]);
  printf("|        | |        |\n");
  printf("|%3.1f  %3.1f| |%3.1f  %3.1f|  back\n", fsv[0][3], fsv[0][2], fsv[1][3], fsv[1][2]);
  printf("+--------+ +--------+\n");
  printf("total: %.1f Newtons, %.1f kilograms\n", newtons, newtons / 9.81);
  printf("----------foot sensors----------\n");
}

static void print_foot_bumpers() {
  int ll = wb_touch_sensor_get_value(lfoot_lbumper);
  int lr = wb_touch_sensor_get_value(lfoot_rbumper);
  int rl = wb_touch_sensor_get_value(rfoot_lbumper);
  int rr = wb_touch_sensor_get_value(rfoot_rbumper);

  printf("----------foot bumpers----------\n");
  printf("   left       right\n");
  printf("+--------+ +--------+\n");
  printf("|%d      %d| |%d      %d|\n", ll, lr, rl, rr);
  printf("|        | |        |\n");
  printf("|        | |        |\n");
  printf("+--------+ +--------+\n");
  printf("----------foot bumpers----------\n");
}

static void print_ultrasound_sensors() {
  double dist[4];
  int i;
  for (i = 0; i < 4; i++)
    dist[i] = wb_distance_sensor_get_value(us[i]);

  printf("-----ultrasound sensors-----\n");
  printf("top:   left: %f m, right %f m\n", dist[2], dist[0]);
  printf("bottom left: %f m, right %f m\n", dist[3], dist[1]);
  printf("-----ultrasound sensors-----\n");
}

static void print_camera_image() {

  const int SCALED = 2;

  int width = wb_camera_get_width(camera);
  int height = wb_camera_get_height(camera);

  // read rgb pixel values from the camera
  const unsigned char *image = wb_camera_get_image(camera);
  
  printf("----------camera image (grey levels)---------\n");
  printf("original resolution: %d x %d, scaled to %d x %d\n",
    width, height, width / SCALED, height / SCALED);
  
  int y, x;
  char line[width / SCALED + 1];
  line[width / SCALED] = 0;  // add line termination
  for (y = 0; y < height; y += SCALED) {
    int count = 0;
    for (x = 0; x < width; x += SCALED) {
      unsigned char grey = wb_camera_image_get_grey(image, width, x, y);
      line[count++] = '0' + grey * 9 / 255;
    }
    line[count++] = 0;
    printf("%s\n", line);
  }
  printf("----------camera image----------\n");
}

static void set_all_leds_color(int rgb) {

  // these leds take RGB values
  int i;
  for (i = 0; i < 5; i++)
    wb_led_set(leds[i], rgb);

  // ear leds are single color (blue)
  // and take values between 0 - 255
  wb_led_set(leds[5], rgb & 0xff);
  wb_led_set(leds[6], rgb & 0xff);
}

// call this function to set the robot position
// [tx ty tz]: the robot's new position, alpha: the robot's heading direction
// For training only: this feature will be disabled during Robotstadium contest rounds
static void send_move_robot_request(double tx, double ty, double tz, double alpha) {
  char request[128];
  sprintf(request, "move robot %s %d %f %f %f %f", my_color_string(), player, tx, ty, tz, alpha);
  wb_emitter_send(super_emitter, request, strlen(request) + 1);
}

// call this function to set the ball position
// [tx ty tz]: the ball's new position
// For training only: this feature will be disabled during Robotstadium contest rounds
static void send_move_ball_request(double tx, double ty, double tz) {
  char request[128];
  sprintf(request, "move ball %f %f %f", tx, ty, tz);
  wb_emitter_send(super_emitter, request, strlen(request) + 1);
}

static void print_help() {
  printf("----------nao_soccer_player_blue.c (demo)----------\n");
  printf("Use the keyboard to control the robots (one at a time)\n");
  printf("(The 3D window need to be focused)\n");
  printf("[1][2][3]: select a player (it will wave hand !)\n");
  printf("[Up][Down]: move one step forward/backwards\n");
  printf("[<-][->]: side step left/right\n");
  printf("[Shift] + [<-][->]: turn left/right\n");
  printf("[U]: print ultrasound sensors\n");
  printf("[A]: print accelerometers\n");
  printf("[G]: print gyros\n");
  printf("[F]: print foot sensors\n");
  printf("[B]: print foot bumpers\n");
  printf("[R]: print RoboCupGameControlData\n");
  printf("[I]: print scaled camera image\n");
  printf("[Home][End]: camera selection (high/low)\n");
  printf("[7][8][9]: change all leds RGB color\n");
  printf("[0]: turn all leds off\n");
  printf("[X]: teleport selected robot\n");
  printf("[Y]: teleport the ball\n");
  printf("[H]: print this help message\n");
  printf("----------nao_soccer_player_blue.c (demo)----------\n");
}

static void terminate() {
  // add you cleanup code here: write results, close files, free memory, etc.
}

static void simulation_step() {
  if (wb_robot_step(TIME_STEP) == -1)
    terminate();
    
  receive_data_packets();
}

static int am_i_selected() {
  return player == selected_player;
} 

static void run_command(int key) {

  if (! am_i_selected())
    return;

  switch (key) {
    case WB_ROBOT_KEYBOARD_LEFT:
      start_motion(side_step_left);
      break;
    case WB_ROBOT_KEYBOARD_RIGHT:
      start_motion(side_step_right);
      break;
    case WB_ROBOT_KEYBOARD_UP:
      start_motion(forwards);
      break;
    case WB_ROBOT_KEYBOARD_DOWN:
      start_motion(backwards);
      break;
    case WB_ROBOT_KEYBOARD_LEFT | WB_ROBOT_KEYBOARD_SHIFT:
      start_motion(turn_left_60);
      break;
    case WB_ROBOT_KEYBOARD_RIGHT | WB_ROBOT_KEYBOARD_SHIFT:
      start_motion(turn_right_60);
      break;
    case 'R':
      print_game_control_data();
      break;
    case 'A':
      print_acceleration();
      break;
    case 'G':
      print_gyro();
      break;
    case 'F':
      print_foot_sensors();
      break;
    case 'B':
      print_foot_bumpers();
      break;
    case 'U':
      print_ultrasound_sensors();
      break;
    case 'I':
      print_camera_image();
      break;
    case WB_ROBOT_KEYBOARD_HOME:
      wb_servo_set_position(camera_select, 0);
      break;
    case WB_ROBOT_KEYBOARD_END:
      wb_servo_set_position(camera_select, 1);
      break;
    case '7':
      set_all_leds_color(0xff0000); // red
      break;
    case '8':
      set_all_leds_color(0x00ff00); // green
      break;
    case '9':
      set_all_leds_color(0x0000ff); // blue
      break;
    case '0':
      set_all_leds_color(0x000000); // off
      break;
    case 'X':
      send_move_robot_request(0.0, 1.0, 0.0, 1.5708);
      break;
    case 'Y':
      send_move_ball_request(0.0, 1.0, 0.0);
      break;
    case 'H':
      print_help();
      break;
  }
}

static void select_player(int p) {
  selected_player = p;
  if (am_i_selected()) {
    printf("selected: %s\n", wb_robot_get_name());
    wbu_motion_play(hand_wave);
  }
}

// main function
int main(int argc, const char *argv[]) {
  
  wb_robot_init();  // call this before any other call to a Webots function
  
  // initialize stuff
  find_and_enable_devices();
  load_motion_files();
  
  // only the selcted robot should display the help message
  if (am_i_selected())
    print_help();

  // walk forwards until a key is pressed
  wbu_motion_set_loop(forwards, true);
  wbu_motion_play(forwards);
  int key = 0;
  do {
    simulation_step();
    key = wb_robot_keyboard_get_key();
  }
  while (! key);
  
  // stop looping this motion
  wbu_motion_set_loop(forwards, false);

  // read keyboard and execute user commands
  while (1) {
    if (key) {
      switch (key) {
        case '1': select_player(GOALIE); break;
        case '2': select_player(PLAYER1); break;
        case '3': select_player(PLAYER2); break;
        default:
          run_command(key);
          break;
      }
    }
    
    simulation_step();
    key = wb_robot_keyboard_get_key();
  }
  
  return 0;
}
