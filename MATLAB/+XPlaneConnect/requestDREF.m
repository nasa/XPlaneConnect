function result = requestDREF( DREFArray, varargin )
%requestDREF request the value of a specific DataRef from X-Plane over UDP
%
%Inputs
%     DREFArray: Cell Array of DataRefs to be requested
%     IP Address (optional): IP Address of the machine that will receive the data as a character array. Default is '127.0.0.1' (local machine)
%     port (optional): Port on the receiving machine where the data will be sent. Default is 49009 (XPlaneConnect). In general use 49009 to send to the plugin
%
%Outputs
%	result: cell array of resulting data where 
%
%Use
%	1. import XPlaneConnect.*;
%   2. DREFArray = {'sim/cockpit2/controls/yoke_heading_ratio','sim/cockpit2/controls/yoke_roll_ratio'};
%	3. result = requestDREF( DREFArray, '172.0.100.54' );
%
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%
%BEGIN CODE

import XPlaneConnect.*

message = zeros(1,6);
len = 7;
status = -1; %#ok<NASGU> % no data

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
        
    % Look for response 
        for i=1:40
            data = readUDP();
            if length(data) > 1 % Received Data
                status = 0;
                counter = 7;
                nArrays = data(6);
                result = cell(nArrays,1);
                for j=1:nArrays
                    sizeArray = data(counter);
                    result{j} = typecast(uint8(data(counter+1:counter+sizeArray*4))','single');
                    counter = counter + 1 + sizeArray * 4;
                end
                break;
            else
                result = cell(0,1);
            end
        end 
end