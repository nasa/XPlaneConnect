function DATATest(  )
%DATATest Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

DREFS = {'sim/flightmodel/engine/ENGN_thro'};

value = rand();
data = struct('h',25,'d',[value,-998,-998,-998,-998,-998,-998,-998]);
 
sendDATA(data);
result = getDREFs(DREFS);

assert(isequal(length(result),8),'DATATest: getDREFs unsucessful-wrong number of elements returned');
assert(abs(result(1)-value)<1e-4,'DATATest: DATA set unsucessful');

end

