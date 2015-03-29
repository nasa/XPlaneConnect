%% X-Plane Connect MATLAB Example Script
% This script demonstrates how to read and write data to the XPC plugin.
% Before running this script, ensure that the XPC plugin is installed and
% X-Plane is running.
%% Import XPC
addpath('../')
import XPlaneConnect.*
%% Setup
% Create variables and open connection to X-Plane

disp('xplaneconnect Example Script\n- Setting up Simulation');
Socket = openUDP(49005);
%% Set position of the player aircraft
disp('Setting position');
%       Lat     Lon         Alt   Pitch Roll Heading Gear
POSI = [37.524, -122.06899, 2500, 0,    0,   0,      1];
sendPOSI(POSI);
pause(5);
posi(1) = 37.52465;   % Move aircraft North a bit
posi(4) = 20;         % Set aircraft pitch up 20 degrees
sendPOSI(POSI);
%% Set rates
disp('Setting rates');
data = struct('h',18,'d',[0,-999,0,-999,-999,-999,-999,-999]);
sendDATA(data);
data = struct('h',3,'d',[3,130,130,130,130,-999,-999,-999]);
sendDATA(data);
data = struct('h',16,'d',[16,0,0,0,-999,-999,-999,-999]);
sendDATA(data);
%                  Alpha Velocity PQR
% data = struct('h',[18,   3,       16],...
%               'd',[[0,-999,0,-999,-999,-999,-999,-999],... % Alpha data
%                    [3,130,130,130,130,-999,-999,-999],...  % Velocity data
%                    [16,0,0,0,-999,-999,-999,-999]]);       % PQR data
% sendDATA(data);
 %% Set CTRL
 %                      Throttle
 CTRL = [-999,-999,-999,0.8,-999];
 sendCTRL(CTRL);
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
 