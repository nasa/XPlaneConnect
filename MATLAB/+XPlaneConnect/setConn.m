function status = setConn( recvPort, IP, port )
% setConn Send a command to set up the port where you will receive data on
% this computer.
%
% Inputs
%     Receiving Port: Port that data will be sent to in the future for this connection
%     IP Address (optional): IP Address of the machine that will receive the data as a character array. Default is '127.0.0.1' (local machine)
%     port (optional): Port on the receiving machine where the data will be sent. Default is 49009 (XPlaneConnect). In general use 49009 to send to the plugin and 49005 to send to the x-plane udp
% 
% Outputs
%     status: If there was an error. status<0 means there was an error.
% 
% Use
%   1. import XPlaneConnect.*
%   2. status = setConn(49011);
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
    message = zeros(1,9);
    
%% Handle Input
    % Optional parameters
        if ~exist('IP','var'), IP = '127.0.0.1'; end
        if ~exist('port','var'), port = 49009; end

    % Check format of input-TODO

%% BODY
    % Header
        message(1:4) = 'CONN'-0;
    
    % RecvPort
        message(6:9) = typecast(uint32(recvPort),'uint8');
        
    % Send
        sendUDP(IP,port,message);

end