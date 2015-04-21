function setConnTest(  )

%setConnTest Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

DREFS = {'sim/cockpit/switches/gear_handle_status'};

setConn(49055);
result = getDREFs(DREFS);

assert(isequal(length(result),1),'setConnTest: requestDREF unsucessful-wrong number of elements returned');

setConn(49009);

end

