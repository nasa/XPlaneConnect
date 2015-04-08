function sendTEXTTest()
%% Setup
addpath('../../MATLAB')
import XPlaneConnect.*

%% Test
sendTEXT('sendTEXT test message M', 200, 400);


end

