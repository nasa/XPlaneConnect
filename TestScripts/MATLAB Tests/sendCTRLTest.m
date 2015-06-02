function sendCTRLTest(  )
%CTRLTest Summary of this function goes here
%   Detailed explanation goes here
%% Test player aircraft
addpath('../../MATLAB')
import XPlaneConnect.*

DREFS = {'sim/cockpit2/controls/yoke_pitch_ratio',...
    'sim/cockpit2/controls/yoke_roll_ratio',...
    'sim/cockpit2/controls/yoke_heading_ratio',...
    'sim/flightmodel/engine/ENGN_thro',...
    'sim/cockpit/switches/gear_handle_status',...
    'sim/flightmodel/controls/flaprqst'};
THROT = rand();
CTRL = [0.0, 0.0, 1.0, THROT, 0.0, 1.0];

sendCTRL(CTRL);
result = getDREFs(DREFS);

assert(isequal(length(result),6),'CTRLTest: getDREFs unsucessful-wrong number of elements returned');
assert(isequal(length(result{4}),8),'CTRLTest: getDREFs unsucessful- element 1 incorrect size (should be size 8)');
for i=1:length(CTRL)-1
    assert(abs(result{i}(1)-CTRL(i))<1e-4,['CTRLTest: DATA set unsucessful-',num2str(i)]);
end

%% Test NPC aircraft
DREFS = {'sim/multiplayer/position/plane1_yolk_pitch',...
    'sim/multiplayer/position/plane1_yolk_roll',...
    'sim/multiplayer/position/plane1_yolk_yaw',...
    'sim/multiplayer/position/plane1_throttle',...
    'sim/multiplayer/position/plane1_gear_deploy',...
    'sim/multiplayer/position/plane1_flap_ratio'};
THROT = rand();
CTRL = [0.0, 0.0, 1.0, THROT, 0.0, 1.0];

sendCTRL(CTRL, 1);
result = getDREFs(DREFS);

assert(isequal(length(result),6),'CTRLTest: requestDREF unsucessful-wrong number of elements returned');
assert(isequal(length(result{4}),8),'CTRLTest: requestDREF unsucessful- element 1 incorrect size (should be size 8)');
for i=1:length(CTRL)-1
    assert(abs(result{i}(1)-CTRL(i))<1e-4,['CTRLTest: DATA set unsucessful-',num2str(i)]);
end

end

