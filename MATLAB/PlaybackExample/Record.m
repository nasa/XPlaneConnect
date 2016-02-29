%% X-Plane Connect MATLAB Recording Example Script
% This script demonstrates how to record data from X-Plane that can later
% be played back. (See Playback.m)
% Before running this script, ensure that the XPC plugin is installed and
% X-Plane is running.
%% Import XPC
addpath('../')
import XPlaneConnect.*

%% Setup
% Create variables and open connection to X-Plane
path = 'MyRecording.txt'; % File to save the data in
interval = 0.1; % Time between snapshots in seconds
duration = 10; % Time to record for in seconds
Socket = openUDP(); % Open connection to X-Plane
fd = fopen(path, 'w'); % Open file

disp('X-Plane Connect Recording Example Script');
fprintf('Recording to ''%s'' for %f seconds in %fs increments.\n', path, duration, interval);

%% Start Recording
count = floor(duration / interval);
for i = 1:count
    posi = getPOSI(0, Socket);
    fprintf(fd, '%f, %f, %f, %f, %f, %f, %f\n', ...
        posi(1), posi(2), posi(3), posi(4), posi(5), posi(6), posi(7));
    pause(interval);
end

%% Close connection and file
closeUDP(Socket);
fclose(fd);

disp('Recording complete.');