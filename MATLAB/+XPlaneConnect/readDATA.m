function [ result ] = readDATA( socket )
% readDATA Reads UDP Socket and interprets data
%
% Inputs
%     socket (optional): The client to read from.
% Outputs
% If data is X-Plane data format:
%     data: Matlab X-Plane DATA Structure
%         .h: Header array containing header numbers corresponding to values in the X-Plane UDP Data screen.
%         .d: 2-D Data array. Each row contains eight values corresponding to the header value (.h(1) corresponds to .d(1,:))
%         .raw: raw UDP data array received by readUDP
% 
% If data is matlab structure:
%     data: Matlab Structure. raw UDP data saved to data.raw
% 
% If data is any other format:
%     data: Matlab Structure containing one field
%         .raw & .d: raw UDP data array received by readUDP
% 
% Use
% 1. import XPlaneConnect.*;
% 2. socket = openUDP(49005);
% 3. data = readDATA(socket);
% 4. status = closeUDP(socket);
% or
% 1. import XPlaneConnect.*;
% 2. data = readDATA(49005);
% 
% NOTE: sending in a port number instead of an opened socket clears the UDP buffer. This only works if the data is being sent at a fast rate. Sending in a UDP Socket reads the first packet in the buffer.
%
% Contributors
%   Christopher Teubert (SGT, Inc.) <christopher.a.teubert@nasa.gov>
%   Jason Watkins <jason.w.watkins@nasa.gov>

import XPlaneConnect.*
    
%% Get client
global clients;
if ~exist('socket', 'var')
    assert(isequal((length(clients) < 2),1), '[readDATA] ERROR: Multiple clients open. You must specify which client to use.');
    if isempty(clients)
    	socket = openUDP(); 
    else
    	socket = clients(1);
    end
end

%% Get data
result.raw = socket.readData();

%% Format data
rows = (length(result.raw) - 5) / 9;
result.h = zeroes(rows);
for i=1:rows
    j = 6 + (i - 1) * 9;
    result.h(i) = result.rows(j);
    result.d = [result.d; result.rows((j+1):(j+9))];
end