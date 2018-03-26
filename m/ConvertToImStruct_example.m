function imStruct = ConvertToImStruct(image, id, distMap)
    
    [rows, colms, ~] = size(image);
    
    % Split into rgb arrays
%     r = image(:,:,1);
%     g = image(:,:,2);
%     b = image(:,:,3);
    
%    r = reshape(image(:,:,1)', [],1)';
%    g = reshape(image(:,:,2)', [],1)';
%    b = reshape(image(:,:,3)', [],1)';   
    
    
%    image_interlaced = zeros(rows*colms*3, 1, 'uint8');
    
%    image_interlaced(1:3:(rows*colms*3)-2) = r;
%    image_interlaced(2:3:(rows*colms*3)-1) = g;
%    image_interlaced(3:3:(rows*colms*3)) = b;
     
 %    for i = 1:rows*colms
 %        image_interlaced((i-1)*3+1) = r(i);    
 %        image_interlaced((i-1)*3+2) = g(i);
 %        image_interlaced((i-1)*3+3) = b(i);
 %    end
     
    % Create image structure:
    f1 = 'rgb';      ... uint8 array
    f2 = 'heatMap';  ... double array
    f3 = 'x';        ... int32
    f4 = 'y';        ... int32
    f5 = 'n';        ... number of color channels
    f6 = 'id';       ... unique id, whole image id := 0

    v1 = image;
    v2 = ones(1,rows*colms, 'double'); ... this needs to be row major
    v3 = int32(colms);
    v4 = int32(rows);
    v5 = int32(3);   ... hard coded as 3 color channels at the moment
    v6 = int32(id);

    imStruct = struct(f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6);    
end