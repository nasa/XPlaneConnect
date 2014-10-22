function [ status ] = selectDATA( index, varargin )
% selectDATA Choose specific X-Plane parameters to be send over UDP
% Version 0.25
% 
% Inputs
%     index: An array of the values that to be sent. Corresponds to the numbers on the XPlane Output screen
%     IP Address (optional): IP Address of the machine that will receive the data as a character array. Default is '127.0.0.1' (local machine)
%     port (optional): Port on the receiving machine where the data will be sent. Default is 49009 (XPlaneConnect). In general use 49009 to send to the plugin and 49005 to send to the x-plane udp
% 
% Outputs
%     status: If there was an error. Status<0 means there was an error.
% 
% Use
% 1. import XPlaneConnect.*;
% 2. values = [1, 2, 3, 27, 40];
% 3. status = selectDATA( values, '127.0.0.1', 49005 ); % send to localhose port 49005
% 
% Change Log
% 10/02/14: [CT] V0.25: Updated to work with updated xpcPlugin
% 04/18/14: [CT] V0.2: Added Versioning
% 06/10/13: [CT] Code created
%
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%
% To Do
% 1. Verify Input
%
% BEGIN CODE

import XPlaneConnect.*

%% Handle Input
p = inputParser;
addRequired(p,'index');
addOptional(p,'IP','127.0.0.1',@ischar);
addOptional(p,'port',49009,@isnumeric);
parse(p,index,varargin{:});

%% BODY
    dataString = ['DSEL'-0,0];
    for i=1:length(index)
        dataString = [dataString, p.Results.index(i), 0, 0, 0];
    end
    status = sendUDP(dataStream, p.Results.IP, p.Results.port);

end

