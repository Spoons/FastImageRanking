%read in the image and the template
function imageStruct = loadImage(filename, id)

image = imread(filename);


lancia_size = size(image);
y = lancia_size(1);
x = lancia_size(2);

%create image structure
f1 = 'rgb'; %uint8 array
f2 = 'heatMap'; %double array
f3 = 'x'; %32bit signed int
f4 = 'y';  %32bit signed int
f5 = 'n'; %32bit signed int
f6 = 'id'; %32bit signed int

v1 = image;
v2 = ones(1,x*y, 'double'); 
v3 = int32(x);
v4 = int32(y);
v5 = int32(3); %3 channels rgb
v6 = int32(id);


imageStruct = struct(f1,v1,f2,v2,f3,v3,f4,v4,f5,v5,f6,v6);


