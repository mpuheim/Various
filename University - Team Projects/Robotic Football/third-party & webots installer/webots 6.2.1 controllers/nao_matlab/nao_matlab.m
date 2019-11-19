% Description: MATLAB controller example for Webots
% File:        nao_matlab.m
% Date:        20-March-2009
% Author:      Yvan Bourquin - http://www.cyberbotics.com

% uncomment the next two lines if you want to use
% MATLAB's desktop and interact with the controller
%desktop;
%keyboard;

% control step
TIME_STEP=40;

% get and enable camera
camera = wb_robot_get_device('camera');
wb_camera_enable(camera,TIME_STEP);

% get and enable accelerometer
accelerometer = wb_robot_get_device('accelerometer');
wb_accelerometer_enable(accelerometer,TIME_STEP);

% make the robot walk continuously
forwards_motion = wbu_motion_new('../motions/Forwards.motion');
wbu_motion_set_loop(forwards_motion, true);
wbu_motion_play(forwards_motion);

% get and enable foot sensors
fsr(1,1) = wb_robot_get_device('LFsrFL');
fsr(1,2) = wb_robot_get_device('LFsrFR');
fsr(1,3) = wb_robot_get_device('LFsrBR');
fsr(1,4) = wb_robot_get_device('LFsrBL');
fsr(2,1) = wb_robot_get_device('RFsrFL');
fsr(2,2) = wb_robot_get_device('RFsrFR');
fsr(2,3) = wb_robot_get_device('RFsrBR');
fsr(2,4) = wb_robot_get_device('RFsrBL');
for i = [1:4]
  wb_touch_sensor_enable(fsr(1,i), TIME_STEP);
  wb_touch_sensor_enable(fsr(2,i), TIME_STEP);
end

% count the steps
steps = 0;

while wb_robot_step(TIME_STEP) ~= -1
  steps = steps + 1;

  % get camera RGB image
  % this function return an image in true color (RGB) uint8 format
  rgb = wb_camera_get_image(camera);

  % display camera image
  subplot(2,2,1);
  image(rgb);
  title('RGB Camera');

  % every 10 time step
  if mod(steps,10) == 0
    % create intensity image from RGB image
    intens = (rgb(:,:,1)+rgb(:,:,2)+rgb(:,:,3))/3;

    % use 'canny' edge detection
    edges = edge(intens,'canny');

    % display 'canny' image
    subplot(2,2,2);
    imshow(edges);
    title('Edge detection');
  end

  % compute total foot pressures
  left(steps) = 0;
  right(steps) = 0;
  for i = [1:4]
    left(steps) = left(steps) + wb_touch_sensor_get_value(fsr(1,i));
    right(steps) = right(steps) + wb_touch_sensor_get_value(fsr(2,i));
  end

  % plot foot pressures
  subplot(2,2,3);
  if steps <= 100
    time = [1:steps] * TIME_STEP / 1000;
    plot(time,left(),'b',time,right(),'r');
  else
    time = [steps-100:steps] * TIME_STEP / 1000;
    plot(time,left(steps-100:steps),'b',time,right(steps-100:steps),'r');
  end
  title('Left/right foot pressure');
  xlabel('time [s]');
  ylabel('Newtons [N]');

  % plot accelerometer values
  acc = wb_accelerometer_get_values(accelerometer);
  subplot(2,2,4);
  bar([1:3], acc);
  title('Accelerometers');
  xlabel('Axes X Y Z');
  ylabel('Acceleration [m/s^2]');
  axis([0.5 3.5 -5 15]);

  % flush graphics
  drawnow;
end

% your cleanup code goes here
