function closeUDP(socket)
% closeUDP Closes the specified connection and releases resources associated with it.
%
% Inputs
%     socket: An XPC client to close
% 
% Use
% 1. import XPlaneConnect.*;
% 2. socket = openUDP();
% 3. closeUDP(socket);
% 
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%   [JW] Jason Watkins
%       jason.w.watkins@nasa.gov

%% Close the socket
assert(isequal(class(socket), 'gov.nasa.xpc.XPlaneConnect'),...
    '[closeUDP] ERROR: socket was not an XPC client.'); 
socket.close;

%% Track open clients
global clients;
for i = 1:length(clients)
    if socket == clients(i)
        clients = [clients(1:i-1) clients(i+1:length(clients))];
    end
end

end