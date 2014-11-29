function setConnTest(  )

disp('Not Functional Yet!')
%setConnTest Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

DREFS = {'sim/operation/override/override_planepath'};
value = 0; % Resume

setConn(value);
result = requestDREF(DREFS);

assert(isequal(length(result),1),'setConnTest: requestDREF unsucessful-wrong number of elements returned');

end

