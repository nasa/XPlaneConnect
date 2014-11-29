function sendReadTest(  )
%SENDREADTEST Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

recvPortNum = 49074;
recvPort = openUDP(49074);

sendUDP('test2'-0,'127.0.0.1',49074);
result = readUDP(recvPort);
closeUDP(recvPort);

assert(~isequal(result,-998), 'no data received');
assert(all(result(1:4)'=='test'-0));

end

