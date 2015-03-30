%% X-Plane Connect MATLAB Example Script
% This script demonstrates how to read and write data to the XPC plugin.
% Before running this script, ensure that the XPC plugin is installed and
% X-Plane is running.
%% Import XPC
addpath('../')
import XPlaneConnect.*
%% Setup
% Create variables and open connection to X-Plane

disp('xplaneconnect Example Script-');
disp('Setting up Simulation');
Socket = openUDP(49005);
%% Set position of the player aircraft
disp('Setting position');
 pauseSim(1);
%       Lat     Lon         Alt   Pitch Roll Heading Gear
POSI = [37.524, -122.06899, 2500, 0,    0,   0,      1];
sendPOSI(POSI); % Set own aircraft position

%       Lat     Lon           Alt   Pitch Roll Heading Gear
POSI = [37.52465, -122.06899, 2500, 0,    20,  0,      1];
sendPOSI(POSI, 1); % Place another aircraft just north of us
%% Set rates
disp('Setting rates');
%                  Alpha Velocity PQR
data = struct('h',[18,   3,       16],...
              'd',[0,-999,0,-999,-999,-999,-999,-999;... % Alpha data
                   130,130,130,130,-999,-999,-999,-999;...  % Velocity data
                   0,0,0,-999,-999,-999,-999,-999]);       % PQR data
sendDATA(data);
 %% Set CTRL
 %                      Throttle
 CTRL = [0,0,0,0.8];
 sendCTRL(CTRL);
 pause(5);
 pauseSim(0);
 pause(5) % Run sim for 5 seconds
 %% Pause sim
 disp('Pausing simulation');
 pauseSim(1);
 pause(5);
 disp('Unpausing simulation');
 pauseSim(0);
 pause(10) % Run sim for 10 seconds
 %% Use DREF to raise landing gear
 disp('Raising gear');
 gearDREF = 'cockpit/switches/gear_handle_status';
 sendDREF(gearDREF, 0);
 pause(10) % Run sim for 10 seconds
 %% Confirm gear and paus status by reading DREFs
 disp('Checking gear');
 pauseDREF = 'sim/operation/override/override_planepath';
 result = requestDREF({gearDREF, pauseDREF});
 if result{1} == 0
     disp('Gear stowed');
 else
     disp('Error stowing gear');
 end
 %% Exit
 closeUDP(Socket);
 disp('--End of example program--');
 