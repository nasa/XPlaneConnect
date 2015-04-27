function openCloseTest()
%OPENCLOSETEST Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

socket = openUDP();
closeUDP(socket);
socket = openUDP();
closeUDP(socket);

end

