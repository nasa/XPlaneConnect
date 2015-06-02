function sendPOSITest(  )
%POSITest Summary of this function goes here
%   Detailed explanation goes here
addpath('../../MATLAB')
import XPlaneConnect.*

DREFS = {'sim/flightmodel/position/latitude', ...
         'sim/flightmodel/position/longitude', ...
         'sim/flightmodel/position/y_agl', ...
         'sim/flightmodel/position/phi', ...
         'sim/flightmodel/position/theta', ...
         'sim/flightmodel/position/psi', ...
         'sim/cockpit/switches/gear_handle_status'};
POSI = [37.524, -122.06899, 2500, 0, 0, 0, 1]; % Gear

sendPOSI(POSI);
result = getDREFs(DREFS);

assert(isequal(length(result),7),'POSITest: getDREFs unsucessful-wrong number of elements returned');
for i=1:length(POSI)
    if i==3
        continue
    end
    assert(abs(result(i)-POSI(i))<1e-4,['POSITest: DATA set unsucessful-',num2str(i)]);
end

end

