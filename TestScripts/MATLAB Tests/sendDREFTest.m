function sendDREFTest(  )
%SENDREADTEST Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

DREFS = {'sim/cockpit/switches/gear_handle_status'};
value = randi([0 10]);

sendDREF(DREFS{1},value);
result = requestDREF(DREFS);

assert(isequal(length(result),1),'sendDREFTest: requestDREF unsucessful-wrong number of elements returned');
assert(isequal(length(result{1}),1),'sendDREFTest: requestDREF unsucessful- element 1 incorrect size (should be size 1)');

assert(isequal(result{1},value),'sendDREFTest: DREF set unsucessful');

end

