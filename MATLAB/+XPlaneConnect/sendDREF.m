function status = sendDREF( dataRef, Value, varargin )
% sendDREF Send a command to change any DataRef in X-Plane over UDP. This requires the X-Plane Connect Plugin to be running
% Version 0.25
%
% Inputs
%     dataRef: The X-Plane Dataref that will be chaged
%     Value: The value that the above dataref is set to
%     IP Address (optional): IP Address of the machine that will receive the data as a character array. Default is '127.0.0.1' (local machine)
%     port (optional): Port on the receiving machine where the data will be sent. Default is 49009 (XPlaneConnect). In general use 49009 to send to the plugin and 49005 to send to the x-plane udp
% 
% Outputs
%     status: If there was an error. status<0 means there was an error.
% 
% Use
%   1. import XPlaneConnect.*
%   2. dataRef  = 'sim/aircraft/parts/acf_gear_deploy'; // Landing Gear
%   3. Value    = 0;
%   4. status = sendDREF(dataRef, Value);
% 
% Change Log
%  10/02/14: [CT] V0.25: Updated to work with updated xpcPlugin
%  04/18/14: [CT] V0.2: First Version
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

status = 0;
    
%% Handle Input
p = inputParser;
addRequired(p,'dataRef');
addRequired(p,'Value');
addOptional(p,'IP','127.0.0.1',@ischar);
addOptional(p,'port',49009,@isnumeric);
parse(p,dataRef, Value ,varargin{:});

%% BODY
    dataStream = zeros(1,7+length(p.Results.dataRef)+length(p.Results.Value)*4);
    % Build Header
        dataStream(1:4) = 'DREF'-0;
    
    % Add DREF
        dataStream(6) = uint8(length(p.Results.dataRef));
        len = 6+length(p.Results.dataRef);
        dataStream(7:len)=p.Results.dataRef-0;
    
    % Add Value
        dataStream(len+1) =uint8(length(p.Results.Value));
        dataStream(len+2:end)=typecast(single(p.Results.Value),'uint8');

    % Send DREF
        if length(dataStream) > 5
            status = sendUDP(dataStream, p.Results.IP, p.Results.port);
        end
end