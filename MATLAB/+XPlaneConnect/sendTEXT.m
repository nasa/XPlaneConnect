function [ status ] = sendTEXT( msg, varargin )
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
addRequired(p,'msg');
addOptional(p,'x',-1,@isnumeric);
addOptional(p,'y',-1,@isnumeric);
addOptional(p,'IP','127.0.0.1',@ischar);
addOptional(p,'port',49009,@isnumeric);
parse(p,msg,varargin{:});

%% Body
    header = ['TEXT'-0,0];
    dataStream = [header,...
        typecast(uint32(p.Results.x), 'uint8'),...
        typecast(uint32(p.Results.y), 'uint8'),...
        uint8(length(msg)), msg-0];
    
    % Send TEXT
    status = sendUDP(dataStream, p.Results.IP, p.Results.port);
end

