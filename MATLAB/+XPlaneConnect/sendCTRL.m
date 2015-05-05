function sendCTRL( values, ac, socket )
% sendCTRL Sends command to X-Plane setting control surfaces on the specified aircraft.
% 
% Inputs
%     values: control array where the elements are as follows:
%           1. Latitudinal Stick [-1,1]
%           2. Longitudinal Stick [-1,1]
%           3. Pedal [-1, 1]
%           4. Throttle [-1, 1]
%           5. Gear (0=up, 1=down)
%           6. Flaps [0, 1]
%           7. Speed Brakes [-0.5, 1.5]
%     ac (optional): The aircraft to set. 0 for the player aircraft.
%     socket (optional): The client to use when sending the command.
%       
% Outputs
%     status: If there was an error. Status<0 means there was an error.
% 
% Use
% 1. import XPlaneConnect.*;
% 2. socket = openUDP();
% 3. status = sendCTRL([0, 0, 0, 0.8, 0, 1], 0, socket); % Set throttle and flaps on the player aircraft.
% 
% Note: send the value -998 to not overwrite that parameter. That is, if
% -998 is sent, the parameter will stay at the current X-Plane value.
% 
% Contributors
%   Christopher Teubert (SGT, Inc.) <christopher.a.teubert@nasa.gov>
%   Jason Watkins <jason.w.watkins@nasa.gov>

import XPlaneConnect.*

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(isequal(length(clients) < 2, 1), '[sendCTRL] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP(); 
    else
    	socket = clients(1);
    end
end

%% Validate input
values = single(values);
if ~exist('ac', 'var')
    ac = 0;
end
ac = logical(ac);

%% Send command
socket.sendCTRL(values, ac);

end