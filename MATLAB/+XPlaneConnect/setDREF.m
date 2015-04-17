function setDREF( dref, value, socket )
% sendDREF Sends a command to X-Plane that sets the given DREF.
%
% Inputs
%     dref: The name of the X-Plane dataref to set.
%     Value: An array of floating point values whose structure depends on the dref specified.
%     socket (optional): The client to use when sending the command.
% 
% Use
%   1. import XPlaneConnect.*
%   2. dataRef  = 'sim/aircraft/parts/acf_gear_deploy'; // Landing Gear
%   3. Value    = 0;
%   4. status = setDREF(dataRef, Value);
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
    assert(istrue(length(clients) < 2), '[setDREF] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
       openUDP(); 
    end
    socket = clients(1);
end

%% Validate input
value = single(value);

%%Send command
socket.setDREF(dref, value);