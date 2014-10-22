function [socket] = openUDP(port, varargin)
%openUDP  Script that opens an UDP Socket
% Version 0.25
%
%Inputs
%   port: UDP Port for socket
%   timeout (optional): Optional parameter for time to UDP timeout (in ms)
%Outputs
%   Socket: UDP Socket
% 
% Use
% 1. import XPlaneConnect.*;
% 2. Socket = openUDP(49005); %Open socket at port 49005 with timeout=0.1 sec
% or
% 2. Socket = openUDP(49005); %Open socket at port 49005 with timeout=0.2 sec
%
%Change Log
%  10/02/14: [CT] V0.25: Updated to work with updated xpcPlugin
%  04/18/14: [CT] Added Versioning
%  09/12/13: [CT] Added optional timeout input argument
%  09/10/13: [CT] Code created
%
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%
% To Do
% 1. Verify Input
%
% BEGIN CODE

import java.net.DatagramSocket

%% create socket
    socket = DatagramSocket(port);
    socket.setSoTimeout(100);
    socket.setReceiveBufferSize(2000);

%% interpret input
if ~isempty(varargin)
    if isnumeric(varargin(1))
        socket.setSoTimeout(varargin(1));
    end
end

end
