function [ data ] = readUDP( input )
%readUDP Read Array from UDP Socket
% 
% Inputs
% 	input: Either an opened UDP Socket or integer port number
% 
% Outputs
%   data: UDP uint8 Array. Equal to -998 in the case of an error
% 
% Use
% 1. import XPlaneConnect.*;
% 2. socket = openUDP(49005);
% 3. data = readUDP(socket);
% 4. status = closeUDP(socket);
% 
% or
% 
% 1. import XPlaneConnect.*;
% 2. data = readUDP(49005);
% 
% NOTE: sending in a port number instead of an opened socket clears the 
%UDP buffer. This only works if the data is being sent at a fast rate. 
%Sending in a UDP Socket reads the first packet in the buffer. 
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
    import java.net.DatagramPacket
    bits = 2000;
    
    %% Interpret Input
    socket = input;
    if isnumeric(input) 
        socket=openUDP(input);
    end
    
    %% Try reading packet
    try
        packet = DatagramPacket(zeros(1,bits,'int8'),bits);
        socket.receive(packet)
        data = packet.getData;
        
        data = int16(data);
        data(data(:)<0) = uint8(data(data(:)<0) + 256); %fix signed issue
        size = int16(data(5)); %size of data stream

        %% trim trailing data
        for i=1:floor(length(data)/256)+1
            if data(size+1:end)==0
                break
            end
            size = size + 256;
        end
        data = data(1:size);
        
    catch err %Read Unsuccessful
        data = -998;
    end
    
    %% Close Port (if opened in code)
    if isnumeric(input)
        socket.close()
    end
end

