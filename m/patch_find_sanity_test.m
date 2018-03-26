image = loadImage('images//impreza_rally.png', 0);
template(1) = loadImage('images//impreza_rally_t.png', 1);

%create location array
%x,y,theta,image id
%this must be in standard (matlab) column major format
i = 1;
loc = ones(image.x * image.y, 4, 'int32');
for x = template(1).x:image.x-template(1).x
    for y = template(1).y:image.y-template(1).y
        loc(i,:) = [x,y,0,1];
        i = i+1;
    end
end
result = quickscore(image, template, loc);

[lowest_result, index] = min(result);
coord = loc(index,:);
