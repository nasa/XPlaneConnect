function openCloseTest()
%OPENCLOSETEST Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

socket = openUDP( 49007 );

closeUDP( socket );

end

