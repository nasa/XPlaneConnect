function struTest( )
%STRUTEST Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

recvNum = 49076;
recvPort = openUDP(recvNum);

testSTRU = struct('h',15,'d',[12, 34, 55, 99, 1, 0],'test','theTest');

sendSTRU(testSTRU,'127.0.0.1',recvNum);
resultSTRU = readDATA(recvPort);
closeUDP(recvPort);
resultSTRU = rmfield(resultSTRU,'raw');
assert(isequal(testSTRU,resultSTRU),'struTest: Error-structs are not equal');

end

