function posi = getPOSI(ac, socket)
% getPOSI Gets position information for the specified aircraft
%
% Inputs
%     ac: The aircraft number to get position data for.
% Outputs
%     posi: An array of values matching the the format used by sendPOSI


import XPlaneConnect.*

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(isequal(length(clients) < 2, 1), '[getPOSI] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP(); 
    else
    	socket = clients(1);
    end
end

%% Validate input
ac = int32(ac);

%% Send command
posi = socket.getPOSI(ac);

end