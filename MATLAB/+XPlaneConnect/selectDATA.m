function selectDATA( rows, socket )
% selectDATA Choose specific X-Plane parameters to be send over UDP
% 
% Inputs
%     rows: An array of the values that to be sent. Corresponds to the numbers on the XPlane Output screen
%     socket (optional): The client to use when sending the command.
% 
% Use
% 1. import XPlaneConnect.*;
% 2. values = [1, 2, 3, 27, 40];
% 3. status = selectDATA( values, '127.0.0.1', 49005 ); % send to localhose port 49005
%
% Contributors
%   Christopher Teubert (SGT, Inc.) <christopher.a.teubert@nasa.gov>
%   Jason Watkins <jason.w.watkins@nasa.gov>

import XPlaneConnect.*

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(isequal((length(clients) < 2),1), '[selectDATA] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP(); 
    else
    	socket = clients(1);
    end
end

%% Validate input
rows = int32(rows);

%% Send command
socket.selectDATA(rows);
