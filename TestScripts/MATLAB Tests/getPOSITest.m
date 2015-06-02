function getPOSITest()
%GETCTRLTEST Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

values = [ 37.524, -122.06899, 2500, 45, -45, 15, 1 ];
pauseSim(1);
sendPOSI(values, 0);
actual = getPOSI(0);
pauseSim(0);

assert(isequal(length(actual), length(values)));
for i = 1:length(actual)
    assert(abs(actual(i) - values(i)) <1e-4)
end
end
