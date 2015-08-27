function ctrl = getCTRL(ac, socket)
% getCTRL Gets control surface information for the specified aircraft
%
% Inputs
%     ac: The aircraft number to get control surface data for.
% Outputs
%     posi: An array of values matching the the format used by sendCTRL


import XPlaneConnect.*

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(isequal(length(clients) < 2, 1), '[getCTRL] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP(); 
    else
    	socket = clients(1);
    end
end

%% Validate input
ac = int32(ac);

%% Send command
ctrl = socket.getCTRL(ac);

end