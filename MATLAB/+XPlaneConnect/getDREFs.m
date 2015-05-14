function result = getDREFs( drefs, socket )
%requestDREF request the value of a specific DataRef from X-Plane over UDP
%
%Inputs
%     drefs: Cell Array of DataRefs to be requested
%     socket (optional): The client to use when sending the command.
%Outputs
%	result: cell array of resulting data.
%
%Use
%	1. import XPlaneConnect.*;
%   2. socket = opendUDP();
%   3. drefs = {'sim/cockpit2/controls/yoke_heading_ratio','sim/cockpit2/controls/yoke_roll_ratio'};
%	4. result = requestDREF(drefs, socket);
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
    assert(isequal(length(clients) < 2, 1), '[getDREFs] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP(); 
    else
    	socket = clients(1);
    end
end

%% Send command
result = socket.getDREFs(drefs);

end