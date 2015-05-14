function sendDREFTest(  )
%SENDREADTEST Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

DREFS = {'sim/cockpit/switches/gear_handle_status'};
value = randi([0 10]);

sendDREF(DREFS{1},value);
result = getDREFs(DREFS);

assert(isequal(length(result),1),'setDREFTest: requestDREF unsucessful-wrong number of elements returned');

assert(isequal(result(1),value),'setDREFTest: DREF set unsucessful');

end

