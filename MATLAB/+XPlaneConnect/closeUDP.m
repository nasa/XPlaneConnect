function [socket] = closeUDP(socket)
% closeUDP Script that closes a UDP Socket
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