function [ status ] = sendPOSI( posi, varargin )
% sendPOSI Send X-Plane Aircraft Position over UDP
% 
% Inputs
%     posi: Position array where the elements are as follows:
%           1. Latitiude (deg)
%           2. Longitude (deg)
%           3. Altitude (m above MSL)
%           4. Roll (deg)
%           5. Pitch (deg)
%           6. True Heading (deg)
%           7. Gear (0=up, 1=down)
%     acft (optional): Aircraft # (default is 0; 0 = own aircraft)
%     IP Address (optional): IP Address of the machine that will receive the data as a character array. Default is '127.0.0.1' (local machine)
%     port (optional): Port on the receiving machine where the data will be sent. Default is 49009 (XPlaneConnect). In general use 49009 to send to the plugin and 49005 to send to the x-plane udp
%       
% Outputs
%     status: If there was an error. Status<0 means there was an error.
% 
% Use
% 1. import XPlaneConnect.*;
% 2. #Send the data array to port 49009 on the computer at IP address 172.0.100.54.
% 3. status = sendPOSI([37.5242422, -122.06899, 2500, 0, 0, 0, 1], 1, '172.0.100.54');
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
addRequired(p,'posi');
addOptional(p,'acft',0,@isnumeric);
addOptional(p,'IP','127.0.0.1',@ischar);
addOptional(p,'port',49009,@isnumeric);
parse(p,posi,varargin{:});

%% BODY
    header = ['POSI'-0,0];
    dataStream = [header, p.Results.acft];
    
    % Deal with position update
    position = [37.4185718,-121.935565,500,0,0,0, 0];
    
    for i=1:min(length(position),length(p.Results.posi))
        position(i) = p.Results.posi(i);
    end
    dataStream = [dataStream, typecast(single(position),'uint8')];
    
    % Send POSI
    status = sendUDP(dataStream, p.Results.IP, p.Results.port);
    
end

