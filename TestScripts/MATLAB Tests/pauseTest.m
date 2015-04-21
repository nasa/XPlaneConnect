function pauseTest(  )
%PauseTest Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

DREFS = {'sim/operation/override/override_planepath'};

value = 1; % Pause

pauseSim(value);
result = getDREFs(DREFS);

assert(abs(result(1)-value)<1e-4,'pauseTest: Pause Unsuccessful');
value = 0; % Resume

pauseSim(value);
result = getDREFs(DREFS);

assert(abs(result(1)-value)<1e-4,'pauseTest: Pause Unsuccessful');

end

