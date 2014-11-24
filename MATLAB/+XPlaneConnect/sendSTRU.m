function [ status ] = sendSTRU( STRU, varargin )
%sendSTRU Send a MATLAB structure over UDP
%
%Inputs
%     stru: A MATLAB structure
%     IP Address (optional): IP Address of the machine that will receive the data as a character array. Default is '127.0.0.1' (local machine)
%     port (optional): Port on the receiving machine where the data will be sent. Default is 49009 (XPlaneConnect). In general use 49009 to send to the plugin
%
%Outputs
%	status: If there was an error. Status<0 means there was an error.
%
%Use
%	1. import XPlaneConnect.*;
%	2. data = struct('a',[1:20],'b',1.853,'name','Example Structure');
%	3. #Send the data structure to port 49005 on the computer at IP address 172.0.100.54
%	4. status = sendUDP( data, '172.0.100.54', 49005 );
%
% Contributors
%   [CT] Christopher Teubert (SGT, Inc.)
%       christopher.a.teubert@nasa.gov
%
% To Do
%
%BEGIN CODE

    import XPlaneConnect.*
    
    %% Handle Input
    p = inputParser;
    addRequired(p,'STRU');
    addOptional(p,'IP','127.0.0.1',@ischar);
    addOptional(p,'port',49009,@isnumeric);
    parse(p,STRU,varargin{:});
    
    %% Form Data Array representing the structure
    DATA = ['STRU'-0,0]; %array header
    fieldName = fieldnames(p.Results.STRU); %all Struct fields
    for i=1:length(fieldName) %for each field
        field = getfield(p.Results.STRU,fieldName{i}); %get field
        if ischar(field) %String
            dim1 = 0; %Indicates string
            dim2 = length(field); %length
            data = field-0; %data
        else %Numeric
            dim1 = size(field,1); %Array Dim1
            dim2 = size(field,2); %Array Dim2
            data = typecast(single(reshape(field',1,dim1*dim2)),'uint8'); %Data
        end

        DATA = [DATA,length(fieldName{i}),fieldName{i}-0,dim1,dim2,data]; %add to array
    end
    
    %% Send Array
    status = sendUDP(DATA, p.Results.IP, p.Results.port);
    
end

