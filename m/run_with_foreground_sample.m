image = loadImage('images//lancia.png', 0);
template(1) = loadImage('images//lancia_t.png', 1);
template(2) = loadImage('images//impreza_rally_t.png', 2);

%create location array
%x,y,theta,image id
%this must be in standard (matlab) column major format
locs = int32([500, 55, 0, 1, 0;
    1000,125,25,1, 0;
    200,400,75,1, 0;
     200,400,75,1, 1]);

img_size = size(image.rgb);

 foreground_input = ones(img_size(:, [1,2]), 'int8');
 
%results are int64
[results, lmao] = quickscore(image, template, locs, int32(2), foreground_input);
