function pauseSim( pause, socket )
%pauseSim Pauses or unpauses X-Plane.
%
%Inputs
%     pause: binary value 0=run, 1=pause
%     socket (optional): The client to use when sending the command.
%
%Use
%	1. import XPlaneConnect.*;
%	2. status = pauseSim(1);
%
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%   [JW] Jason Watkins
%       jason.w.watkins@nasa.gov

import XPlaneConnect.*

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(isequal(length(clients) < 2, 1), '[pauseSim] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP(); 
    else
    	socket = clients(1);
    end
end

%% Validate input
pause = int32(pause);

%% Send command
socket.pauseSim(pause);

end