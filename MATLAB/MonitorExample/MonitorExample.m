%% X-Plane Connect MATLAB Example Script
% This script demonstrates how to read and write data to the XPC plugin.
% Before running this script, ensure that the XPC plugin is installed and
% X-Plane is running.
%% Import XPC
clear all
addpath('../')
import XPlaneConnect.*

Socket = openUDP();
while 1
    posi = getPOSI(0, Socket);
    ctrl = getCTRL(0, Socket);

    fprintf('Loc: (%4f, %4f, %4f) Aileron:%2f Elevator:%2f Rudder:%2f\n', ...
    posi(1), posi(2), posi(3), ctrl(2), ctrl(1), ctrl(3));
    pause(0.1);
end
closeUDP(Socket);
