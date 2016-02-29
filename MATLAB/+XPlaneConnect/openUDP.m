function [ socket ] = openUDP(varargin)
%openUDP  Initializes a new instance of the XPC client and returns it.
%
%Inputs
%   xpHost: The network host on which X-Plane is running.
%   xpPort: The port on which the X-Plane Connect plugin is listening.
%   port: The local port to use when sending and receiving data from XPC.
%   timeout (optional): Optional parameter for time to UDP timeout (in ms)
%Outputs
%   socket: An instance of the XPC client.
% 
% Use
% 1. import XPlaneConnect.*;
% 2. socket = openUDP(); % Open a socket using the default settings
% or
% 2. Socket = openUDP('127.0.0.1', 49008, 49005); % Open a socket to connect to X-Plane on the local machine and receive data on port 49005
%
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%   [JW] Jason Watkins
%       jason.w.watkins@nasa.gov

%% Handle input
p = inputParser;
addOptional(p,'xpHost','127.0.0.1',@ischar);
addOptional(p,'xpPort',49009,@isnumeric);
addOptional(p,'port',0,@isnumeric);
addOptional(p,'timeout',100,@isnumeric);
parse(p,varargin{:});

%% Create client
if ~exist('gov.nasa.xpc.XPlaneConnect', 'class')
    [folder, ~, ~] = fileparts(which('XPlaneConnect.openUDP'));
    javaaddpath(fullfile(folder, 'XPlaneConnect.jar'));
end
socket = gov.nasa.xpc.XPlaneConnect(p.Results.xpHost, p.Results.xpPort, p.Results.port, p.Results.timeout);

%% Track open clients
global clients;
clients = [clients, socket];

end