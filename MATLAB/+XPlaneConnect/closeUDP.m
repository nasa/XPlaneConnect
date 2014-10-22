function [socket] = closeUDP(socket)
% closeUDP Script that closes a UDP Socket
% Version 0.25
%
% Inputs
%     Socket: UDP Socket to be closed
% 
% Outputs
%     Socket: Closed Socket
% 
% Use
% 1. import XPlaneConnect.*;
% 2. Socket = openUDP(49005);
% 3. Status = closeUDP(Socket);
% 
% Change Log
%  10/02/14: [CT] V0.25: Updated to work with updated xpcPlugin
%  04/18/14: [CT] V0.2: Added Versioning
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

socket.close;

end