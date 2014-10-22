function status = requestDREF( DREFArray, varargin )
%requestDREF request the value of a specific DataRef from X-Plane over UDP
% Version 0.25
%
%Inputs
%     DREFArray: Cell Array of DataRefs to be requested
%     IP Address (optional): IP Address of the machine that will receive the data as a character array. Default is '127.0.0.1' (local machine)
%     port (optional): Port on the receiving machine where the data will be sent. Default is 49009 (XPlaneConnect). In general use 49009 to send to the plugin
%
%Outputs
%	status: If there was an error. Status<0 means there was an error.
%
%Use
%	1. import XPlaneConnect.*;
%   2. DREFArray = {'sim/cockpit2/controls/yoke_heading_ratio','sim/cockpit2/controls/yoke_roll_ratio'};
%	3. status = requestDREF( DREFArray, '172.0.100.54' );
%
%Change Log
%  10/02/14: [CT] V0.25: Updated to work with updated xpcPlugin
%  04/19/14: [CT] V0.2: First Created
%
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%
% To Do
% 1. Complete- Receive response 
%
%BEGIN CODE

disp('This Function is not functional yet-use the C version. Sorry for any inconvenience')
import XPlaneConnect.*

message = zeros(1,6);
len = 7;

%% Handle Input
p = inputParser;
addRequired(p,'DREFArray');
addOptional(p,'IP','127.0.0.1',@ischar);
addOptional(p,'port',49009,@isnumeric);
parse(p,DREFArray,varargin{:});

%% BODY
    % Header
        message(1:4) = 'GETD'-0;
        message(6) = length(p.Results.DREFArray);
    
    % DREFS
        for i=1:length(p.Results.DREFArray)
            message(len) = length(p.Results.DREFArray{i});
            message(len+1:len+message(len)) = p.Results.DREFArray{i};
            len = len+1+message(len);
        end
        
    % Send UDP
        status = sendUDP(message, p.Results.IP, p.Results.port);
                
end