function sendVIEW(view, socket)
% sendVIEW Sets the camera
%
%Inputs
%     view: The view to use.
%     socket (optional): The client to use when sending the command.
%
%Use
%	1. import XPlaneConnect.*;
%	2. sendView(Forwards);
%
% Contributors
%   [JW] Jason Watkins <jason.w.watkins@nasa.gov>

import XPlaneConnect.*

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(isequal(length(clients) < 2, 1), '[pauseSim] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP(); 
    else
    	socket = clients(1);
    end
end

%% Validate input


%% Send command
socket.sendVIEW(view)

end

