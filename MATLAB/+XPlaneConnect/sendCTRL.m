function [ status ] = sendCTRL( ctrl, acft, IP, port )
% sendCTRL Send X-Plane Aircraft Control Commands over UDP
% 
% Inputs
%     ctrl: control array where the elements are as follows:
%           1. Latitudinal Stick [-1,1]
%           2. Longitudinal Stick [-1,1]
%           3. Pedal [-1, 1]
%           4. Throttle [-1, 1]
%           5. Gear (0=up, 1=down)
%           6. Flaps [0, 1]
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
% 3. status = sendCTRL([0, 0, 0, 0.8, 0, 1], 1, '172.0.100.54',49009);
% 
% Note: send the value -998 to not overwrite that parameter. That is, if -998 is sent, the parameter will stay at the current X-Plane value
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
    % Optional parameters
    if ~exist('IP','var'), IP = '127.0.0.1'; end
    if ~exist('port','var'), port = 49009; end
    if ~exist('acft','var'), acft = 0; end

    % Check format of input-TODO

%%BODY
    header = ['CTRL'-0,0];
    dataStream = header; %TODO-ADD ACFT
    
    % Deal with position update
    control = [-998.5, -998.5, -998.5, -998.5, -998.5, -998.5];
    
    for i=1:min(length(ctrl),length(control))
        control(i) = ctrl(i);
    end
    dataStream = [dataStream, typecast(single(control(1:4)),'uint8')];
    dataStream = [dataStream, uint8(control(5))];
    dataStream = [dataStream, typecast(single(control(6)),'uint8')];
    dataStream = [dataStream, uint8(acft)];
    
    % Send DATA
    status = sendUDP(dataStream, IP, port);

end

