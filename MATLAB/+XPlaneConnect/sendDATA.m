function sendDATA(data, socket)
% sendDATA Send X-Plane formatted DATA over UDP
% 
% Inputs
%     data: X-Plane formatted data. Is a matlab structure with the following fields:
%         .h: Header array containing header numbers corresponding to values in the X-Plane UDP Data screen.
%         .d: 2-D Data array. Each row contains eight values corresponding to the header value (.h(1) corresponds to .d(1,:))
%     socket (optional): The client to use when sending the command.
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

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(isequal(length(clients) < 2, 1), '[sendDATA] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP(); 
    else
    	socket = clients(1);
    end
end

%% Validate input
javaData = [];
for i = 1:length(data.h);
   javaData = [javaData; data.h(i) data.d(i, 1:8)]; 
end

%% Send command
socket.sendDATA(javaData);

