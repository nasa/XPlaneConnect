function sendTEXT( msg, x, y, socket )
% sendTEXT Sends a string to be displayed in X-Plane.
%
% Inputs
%     msg: The string to be displayed
%     x (optional): The distance from the left edge of the screen to display the message.
%     y (optional): The distance from the bottom edge of the screen to display the message.
%     IP Address (optional): IP Address of the machine that will receive the data as a character array. Default is '127.0.0.1' (local machine)
%     port (optional): Port on the receiving machine where the data will be sent. Default is 49009 (XPlaneConnect). In general use 49009 to send to the plugin and 49005 to send to the x-plane udp
%
% Outputs
%     status: 0 if successful, otherwise a negative value.
% 
% Use
% 1. import XPlaneConnect.*;
% 2. % Set a message to be displayed near the top middle of the screen.
% 3. status = sendTEXT('Some text', 512, 600);
% 
% Contributors
%   Jason Watkins (jason.w.watkins@nasa.gov)

import XPlaneConnect.*

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(isequal(length(clients) < 2, 1), '[sendTEXT] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP(); 
    else
    	socket = clients(1);
    end
end

%% Validate input
if ~exist('x', 'var')
    x = -1;
end
if ~exist('y', 'var')
    y = -1;
end
x = int32(x);
y = int32(y);

%% Send command
socket.sendTEXT(msg, x, y);
end