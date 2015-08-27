function getCTRLTest()
%GETCTRLTEST Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

values = [10.0, 5.0, -5.0, 0.8, 1.0, 0.5, -1.5];
sendCTRL(values, 0);
actual = getCTRL(0);

assert(isequal(length(actual), length(values)));
for i = 1:length(actual)
    assert(abs(actual(i) - values(i)) <1e-4)
end
end
