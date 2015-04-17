function setConn(port, socket)
% setConn Send a command to set up the port where you will receive data on
% this computer.
%
% Inputs
%     port: Port that data will be sent to in the future for this connection.
%     socket (optional): The client to use when sending the command.
% 
% Use
%   1. import XPlaneConnect.*
%   2. status = setConn(49011);
%  
% Contributors
%   Christopher Teubert (SGT, Inc.) <christopher.a.teubert@nasa.gov>
%   Jason Watkins <jason.w.watkins@nasa.gov>

import XPlaneConnect.*

%% Get client
global clients;
if ~exist('socket', 'var')
    assert(istrue(length(clients) < 2), '[sendCTRL] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
       openUDP(); 
    end
    socket = clients(1);
end

%% Validate input
port = int32(port);

%% Send command
socket.setCONN(port);

end