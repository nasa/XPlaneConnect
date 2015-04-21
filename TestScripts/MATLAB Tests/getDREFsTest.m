function getDREFsTest(  )
%SENDREADTEST Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

DREFS = {'sim/cockpit/switches/gear_handle_status',...
    'sim/cockpit2/switches/panel_brightness_ratio'};

result = getDREFs(DREFS);

assert(isequal(length(result),2));
assert(isequal(length(result{1}),1));
assert(isequal(length(result{2}),4));

end

