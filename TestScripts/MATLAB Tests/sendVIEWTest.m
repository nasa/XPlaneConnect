function sendVIEWTest()
%% Setup
addpath('../../MATLAB')
import XPlaneConnect.*
    
if ~exist('gov.nasa.xpc.ViewType', 'class')
    [folder, ~, ~] = fileparts(which('XPlaneConnect.openUDP'));
    javaaddpath(fullfile(folder, 'XPlaneConnect.jar'));
end

dref = 'sim/graphics/view/view_type';
fwd = 1000;
chase = 1017;

%% Excecute
sendVIEW(gov.nasa.xpc.ViewType.Forwards);
result = getDREFs(dref);
assert(isequal(result, fwd))

sendVIEW(gov.nasa.xpc.ViewType.Chase);
result = getDREFs(dref);
assert(isequal(result, chase))


end

