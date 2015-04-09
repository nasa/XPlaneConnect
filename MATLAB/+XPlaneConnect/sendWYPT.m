function [ status ] = sendWYPT( op, points, varargin )
% sendWYPT Adds, removes, or clears a set of waypoints to be rendered in
% the simulator.
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
% 2. #Set a message to be displayed near the top middle of the screen.
% 3. status = sendTEXT('Some text', 512, 600);
% 
% Contributors
%   Jason Watkins
%       jason.w.watkins@nasa.gov
%
% To Do
%
% BEGIN CODE

import XPlaneConnect.*
%% Handle Input
p = inputParser;
addRequired(p,'op');
addRequired(p,'points');
addOptional(p,'IP','127.0.0.1',@ischar);
addOptional(p,'port',49009,@isnumeric);
parse(p,op,points,varargin{:});

%% Validate Input
len = uint32(length(points));
assert(op > 0 && op < 4);
assert(mod(len, 3) == 0);
assert(len / 3 < 20);

%% Body
header = ['WYPT'-0,0];
dataStream = [header,...
    uint8(op),...
    uint8(len / 3),...
    typecast(single(points), 'uint8')];

% Send TEXT
status = sendUDP(dataStream, p.Results.IP, p.Results.port);
end

