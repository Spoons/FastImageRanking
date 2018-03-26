function [heatmap] = test_rotation(rotate)
%rotate_test_compile;
clear new_image;
%image = loadImage('images//impreza_rally_t.png',0);
image = loadImage('images//lancia_t.png',0);


[new_image, heatmap] = rotate_image(image.rgb, image.x, image.y, int32(rotate));
imshow(new_image);