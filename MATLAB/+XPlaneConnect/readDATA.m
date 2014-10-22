function [ sensor ] = readDATA( socket )
% readDATA Reads UDP Socket and interprets data
% Version 0.25
%
% Inputs
%     location: Either an opened UDP Socket or integer port number
% 
% Outputs
% If data is X-Plane data format:
%     data: Matlab X-Plane DATA Structure
%         .h: Header array containing header numbers corresponding to values in the X-Plane UDP Data screen.
%         .d: 2-D Data array. Each row contains eight values corresponding to the header value (.h(1) corresponds to .d(1,:))
%         .raw: raw UDP data array received by readUDP
% 
% If data is matlab structure:
%     data: Matlab Structure. raw UDP data saved to data.raw
% 
% If data is any other format:
%     data: Matlab Structure containing one field
%         .raw & .d: raw UDP data array received by readUDP
% 
% Use
% 1. import XPlaneConnect.*;
% 2. socket = openUDP(49005);
% 3. data = readDATA(socket);
% 4. status = closeUDP(socket);
% or
% 1. import XPlaneConnect.*;
% 2. data = readDATA(49005);
% 
% NOTE: sending in a port number instead of an opened socket clears the UDP buffer. This only works if the data is being sent at a fast rate. Sending in a UDP Socket reads the first packet in the buffer.
% 
% Change Log
%  10/02/14: [CT] V0.25: Updated to work with updated xpcPlugin
%  04/18/14: [CT] V0.2: Updated to work with new Plugin
%  09/10/13: [CT] Updated to receive UDP socket or port number
%  06/10/13: [CT] Code created
% 
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%
% To Do
% 1. Verify Input
%
% BEGIN CODE

    import XPlaneConnect.*
    
    %% Read UDP Socket
    [ sensor.raw] = readUDP(socket);
    
    %% Interpret Input
    bits = size(sensor.raw);
    if sensor.raw ~= -998 %If the signal exists
        header = char(sensor.raw(1:4)');
        if strcmp(header,'DATA') %DATA signal type
            Values = floor((bits-5)/36);
            sensor.d = [];
            sensor.h = zeros(Values(1),1);
            for i=1:Values(1)
                sensor.h(i) = sensor.raw(6+(i-1)*36);
                sensor.d = [sensor.d; typecast(uint8(sensor.raw(10+(i-1)*36:5+i*36))','single')];
            end
        elseif strcmp(header,'STRU') %STRU signal type
            a = 6;
            while a<length(sensor.raw)
                strdim = sensor.raw(a);
                if strdim == 0
                    break
                end
                fieldName = char(sensor.raw(a+1:a+strdim)');
                a = a+strdim+1;
                dim1 = sensor.raw(a);
                dim2 = sensor.raw(a+1);
                if dim1 == 0 %String                    
                    value = char(sensor.raw(a+2:a+1+dim2));
                    a = a + dim2 + 2;
                else
                    value = [];
                    for i=1:dim1
                        value(i,:) = typecast(uint8(sensor.raw(a+2+(i-1)*dim2*4:a+1+i*dim2*4))','single');
                    end
                    a = a + dim1*dim2*4+2;
                end
                sensor.(fieldName) = value;
            end
        elseif strcmp(header,'OTHR')
            sensor.d = sensor.raw(6:end);
        else %Other signal type
            sensor.d = sensor.raw;
        end
    else %No Signal
        sensor.d = -998;
    end

end

