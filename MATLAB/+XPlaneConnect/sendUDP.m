function [ status ] = sendUDP( data, IP, port )
%sendUDP Send an one dimensional array of type uint8 data over an UDP connection using a java DatagramSocket
% Version 0.25
%
%Inputs
%     Data: 1-D array of type uint8 data to be sent
%     IP Address (optional): IP Address of the machine that will receive the data as a character array. Default is '127.0.0.1' (local machine)
%     port (optional): Port on the receiving machine where the data will be sent. Default is 49009 (XPlaneConnect). In general use 49009 to send to the plugin
%
%Outputs
%	status: If there was an error. Status<0 means there was an error.
%
%Use
%	1. import XPlaneConnect.*;
%	2. data = uint8([1:20]);
%	3. #Send the data array to port 49005 on the computer at IP address 172.0.100.54.
%	4. status = sendUDP( data, '172.0.100.54', 49005 );
%
%Change Log
%  10/02/14: [CT] V0.25: Updated to work with updated xpcPlugin
%  09/25/14: [CT] V0.24: Added persistant socket
%  04/21/14: [CT] V0.2:Added Versioning, support for DREF/SIMU/GETD/CONN
%  06/10/13: [CT] Code created
%
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%
% To Do
% 1. Verify Input
%
%BEGIN CODE

	import java.net.DatagramSocket
	import java.net.DatagramPacket
	import java.net.InetAddress
	
	data(5) = length(data);
	status = 0;
	
	%% Send array
    persistent socket
    if isempty(socket)
        try
            socket = DatagramSocket;
        catch err
            status = 1; 
            disp(err)
        end
    end
    
	try 
        IP = InetAddress.getByName(IP);
        packet = DatagramPacket(data, length(data), IP, port); %create packet
	    socket.send(packet);
    catch err
	    status = 1; 
	    disp(err)
	end
	    
	

end

