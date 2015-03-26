function pauseTest(  )
%PauseTest Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

DREFS = {'sim/operation/override/override_planepath'};

value = 1; % Pause

pauseSim(value);
result = requestDREF(DREFS);

assert(isequal(length(result),1),'pauseTest: requestDREF unsucessful-wrong number of elements returned');
assert(abs(result{1}(1)-value)<1e-4,'pauseTest: Pause Unsuccessful');
value = 0; % Resume

pauseSim(value);
result = requestDREF(DREFS);

assert(isequal(length(result),1),'pauseTest: requestDREF unsucessful-wrong number of elements returned');
assert(abs(result{1}(1)-value)<1e-4,'pauseTest: Pause Unsuccessful');

end

