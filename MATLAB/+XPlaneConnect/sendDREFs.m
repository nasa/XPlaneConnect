function sendDREFs( drefs, values, socket )
% sendDREFs Sends a command to X-Plane that sets the given DREF(s).
%
% Inputs
%     dref: The name or names of the X-Plane dataref(s) to set.
%     Value: An array or an multidimensional array of floating point values
%            whose structure depends on the dref specified.
%     socket (optional): The client to use when sending the command.
% 
% Use
%   1. import XPlaneConnect.*
%   2. dataRef  = 'sim/aircraft/parts/acf_gear_deploy'; // Landing Gear
%   3. Value    = 0;
%   4. status = setDREF(dataRef, Value);
%  
% Contributors
%   [JW] Jason Watkins
%       jason.w.watkins@nasa.gov

import XPlaneConnect.*

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(isequal(length(clients) < 2, 1), '[setDREF] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP(); 
    else
    	socket = clients(1);
    end
end

%% Validate input
values = single(values);

%%Send command
socket.sendDREFs(drefs, values);