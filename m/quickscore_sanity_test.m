image = loadImage('images//lancia.png', 0);
template(1) = loadImage('images//lancia_t.png', 1);
template(2) = loadImage('images//impreza_rally_t.png', 2);

%create location array
%x,y,theta,image id
%this must be in standard (matlab) column major format
locs = int32([50, 55, 0, 1;
    100,125,25,1;
    200,225,25,1;
    0,0,0,2;
    0,50,5,2]);
    
%results are int64
results = quickscore(image, template, locs);
