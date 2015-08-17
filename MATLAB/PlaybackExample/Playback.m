%% X-Plane Connect MATLAB Recording Example Script
% This script demonstrates how to playback recorded data from X-Plane.
% (See Record.m)
% Before running this script, ensure that the XPC plugin is installed and
% X-Plane is running.
%% Import XPC
addpath('../')
import XPlaneConnect.*

%% Setup
% Create variables and open connection to X-Plane
path = 'MyRecording.txt'; % File containing stored data
interval = 0.1; % Time between snapshots in seconds
Socket = openUDP(); % Open connection to X-Plane
fd = fopen(path, 'r'); % Open file

disp('X-Plane Connect Playback Example Script');
fprintf('Playing back ''%s'' in %fs increments.\n', path, interval);

%% Start Recording
count = floor(duration / interval);
for i = 1:count
    posi = fscanf(fd, '%f, %f, %f, %f, %f, %f, %f\n');
    sendPOSI(posi);
    pause(interval);
end

%% Close connection and file
closeUDP(Socket);
fclose(fd);

disp('Recording complete.');