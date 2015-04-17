function sendWYPT( op, points, socket )
% sendWYPT Adds, removes, or clears a set of waypoints to be rendered in
% the simulator.
%
% Inputs
%     op: The operation to perform. 1=add, 2=remove, 3=clear.
%     points: An array of values representing points. Each triplet in the
%             array will be interpreted as a (Lat, Lon, Alt) point.
%     socket (optional): The client to use when sending the command.
% 
% Use
% 1. import XPlaneConnect.*;
% 2. %Set a message to be displayed near the top middle of the screen.
% 3. status = sendTEXT('Some text', 512, 600);
% 
% Contributors
%   Jason Watkins (jason.w.watkins@nasa.gov)
%
% To Do
%
% BEGIN CODE

import XPlaneConnect.*

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(istrue(length(clients) < 2), '[sendCTRL] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
       openUDP(); 
    end
    socket = clients(1);
end

%% Validate input
len = uint32(length(points));
assert(op > 0 && op < 4);
assert(mod(len, 3) == 0);

%% Send command
socket.sendCTRL(ctrl, ac);

end