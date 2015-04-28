testsPassed=0;
testsFailed=0;
if ismac()
    os = 'Mac';
elseif ispc()
    os = 'Win';
else
    os = 'Linux';
end

disp(['XPC Tests-MATLAB (', os, ')']);

theTests = {{@openCloseTest, 'Open/Close Test', 0},...
    {@sendTEXTTest,'TEXT Test', 0},...
    {@getDREFsTest,'Request DREF Test', 0},...
    {@sendDREFTest,'Send DREF Test', 0},...
    {@DATATest,'DATA Test', 0},...
    {@CTRLTest,'CTRL Test', 0},...
    {@POSITest,'POSI Test', 0},...
    {@sendWYPTTest,'WYPT Test', 0},...
    {@pauseTest,'Pause Test', 0},...
    {@setConnTest, 'setConn Test', 0}};

for i=1:length(theTests)
    fprintf(['Test ',num2str(i),': ',theTests{i}{2},' - ']);
    try
        theTests{i}{1}();
        disp('SUCCESS');
        theTests{i}{3} = 1;
        testsPassed = testsPassed+1;
    catch err
        disp('FAILURE');
        fprintf('\t%s\n',err.message);
        theTests{i}{3} = -1;
        testsFailed = testsFailed + 1;
    end
end

disp('Results Summary:');
fprintf('Passed: %i\tFailed: %i\n',testsPassed, testsFailed);