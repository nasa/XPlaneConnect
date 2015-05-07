function sendDREF( dref, value, socket )
% sendDREFs Sends a command to X-Plane that sets the given DREF. This
% function is now an alias to sendDREFs. It is kept only for backwards
% compatibility.
%
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%   [JW] Jason Watkins
%       jason.w.watkins@nasa.gov

import XPlaneConnect.*

if ~exist('socket', 'var')
    sendDREFs(dref, value)
else
    sendDREFs(dref, value, socket)
end