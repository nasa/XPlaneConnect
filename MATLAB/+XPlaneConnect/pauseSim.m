function status = pauseSim( pause, varargin )
%pauseSim pause Simulation
% Version 0.25
%
%Inputs
%     Pause: binary value 0=run, 1=pause
%     IP Address (optional): IP Address of the machine that will receive the data as a character array. Default is '127.0.0.1' (local machine)
%     port (optional): Port on the receiving machine where the data will be sent. Default is 49009 (XPlaneConnect). In general use 49009 to send to the plugin
%
%Outputs
%	status: If there was an error. Status<0 means there was an error.
%
%Use
%	1. import XPlaneConnect.*;
%	2. status = pauseSim(1);
%
%Change Log
%   10/02/14: [CT] V0.25: Updated to work with updated xpcPlugin
%   04/19/14: [CT] V0.2: First Created
%
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%
% To Do
% 1. Verify Input
%
%BEGIN CODE

import XPlaneConnect.*

%% Handle Input
    % Optional parameters
    p = inputParser;
    addRequired(p,'pause',@isnumeric);
    addOptional(p,'IP','127.0.0.1',@ischar);
    addOptional(p,'port',49009,@isnumeric);
    parse(p,pause,varargin{:});

    % Check format of input-TODO

%% BODY
    message = zeros(1,7);
    
    message(1:4) = 'SIMU'-0;
    message(6) = uint8(p.Results.pause);
    message(7) = 0;

    status = sendUDP(message, p.Results.IP, p.Results.port);
end