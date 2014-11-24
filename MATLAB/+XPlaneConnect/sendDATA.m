function [ status ] = sendDATA(data, varargin)
% sendDATA Send X-Plane formatted DATA over UDP
% 
% Inputs
%     data: X-Plane formatted data. Is a matlab structure with the following fields:
%         .h: Header array containing header numbers corresponding to values in the X-Plane UDP Data screen.
%         .d: 2-D Data array. Each row contains eight values corresponding to the header value (.h(1) corresponds to .d(1,:))
%     IP Address (optional): IP Address of the machine that will receive the data as a character array. Default is '127.0.0.1' (local machine)
%     port (optional): Port on the receiving machine where the data will be sent . Default is 49009 (XPlaneConnect). In general use 49009 to send to the plugin and 49005 to send to the x-plane udp interface.
% 
% Outputs
%     status: 0: no error, <0: error
% 
% Use
% 1. import XPlaneConnect.*;
% 2. data = struct('h',27,'d',[1,-998,-998,-998,-998,-998,-998,-998]);
% 3. %Send the data array to port 49005 on the computer at IP address 172.0.100.54.
% 4. status = sendDATA(data, '172.0.100.54', 49005);
% 
% Note: send the value -998 to not overwrite that parameter. That is, if -998 is sent, the parameter will stay at the current X-Plane value
% 
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%
% To Do
%
% BEGIN CODE

import XPlaneConnect.*

%% Handle Input
p = inputParser;
addRequired(p,'data');
addOptional(p,'IP','127.0.0.1',@ischar);
addOptional(p,'port',49009,@isnumeric);
parse(p,data,varargin{:});

%% BODY

    header = ['DATA'-0,0];
    dataStream = header;
    
    for i=1:length(p.Results.data.h)
        dataStream = [dataStream, p.Results.data.h(i), 0, 0, 0, typecast(single(p.Results.data.d(i,1:8)),'uint8')]; %#ok<AGROW>
    end
    
    % Send DATA
    if length(dataStream) > 5
        status = sendUDP(dataStream, p.Results.IP, p.Results.port);
    else
        disp('Warning in sendDATA: Sending empty dataStream')
        status = -2;
    end

end

