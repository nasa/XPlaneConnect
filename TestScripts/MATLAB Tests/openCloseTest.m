function openCloseTest()
%OPENCLOSETEST Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

socket = openUDP( 49007 );

closeUDP( socket );
assert(isequal(socket.isClosed(),1),'openCloseTest: socket is still open');

socket = openUDP( 49007 );
closeUDP( socket );

end

