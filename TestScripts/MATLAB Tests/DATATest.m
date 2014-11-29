function DATATest(  )
%DATATest Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

DREFS = {'sim/flightmodel/engine/ENGN_thro'};

value = rand();
data = struct('h',25,'d',[value,-998,-998,-998,-998,-998,-998,-998]);
 
sendDATA(data);
result = requestDREF(DREFS);

assert(isequal(length(result),1),'DATATest: requestDREF unsucessful-wrong number of elements returned');
assert(isequal(length(result{1}),8),'DATATest: requestDREF unsucessful- element 1 incorrect size (should be size 8)');
assert(abs(result{1}(1)-value)<1e-4,'DATATest: DATA set unsucessful');

end

