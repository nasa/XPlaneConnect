function sendPOSI( posi, ac, socket )
% sendPOSI Sets the position of the specified aircraft.
%
% Inputs
%     posi: Position array where the elements are as follows:
%           1. Latitiude (deg)
%           2. Longitude (deg)
%           3. Altitude (m above MSL)
%           4. Roll (deg)
%           5. Pitch (deg)
%           6. True Heading (deg)
%           7. Gear (0=up, 1=down)
%     acft (optional): The aircraft to set. 0 for the player aircraft.
%     socket (optional): The client to use when sending the command.
%
% Use
%   1. import XPlaneConnect.*;
%   2. sendPOSI([37.5242422, -122.06899, 2500, 0, 0, 0, 1], 1);
%
% Note: send the value -998 to not overwrite that parameter. That is, if
% -998 is sent, the parameter will stay at the current X-Plane value.
%
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%   [JW] Jason Watkins
%       jason.w.watkins@nasa.gov

import XPlaneConnect.*

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(isequal(length(clients) < 2, 1), '[sendPOSI] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP();
    else
    	socket = clients(1);
    end
end

%% Validate input
posi = double(posi);
if ~exist('ac', 'var')
    ac = 0;
end
ac = logical(ac);

%% Send command
socket.sendPOSI(posi, ac);

end
