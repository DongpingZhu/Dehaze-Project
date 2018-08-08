function [image dc t r] = main(loc)
    image = im2double(imread(loc));
    %image = im2double(imread('Untitled.png'));
    total = 0;
    start = clock;
    dc = dark_channel(image);
    start = clock - start;
    total = total + start;
    fprintf('dark channel %u:%f\r', start(5), start(6)); 
    start = clock;
    al = estimate_atmospheric_light(dc, image);
    start = clock - start;
    total = total +  start;
    fprintf('atmospheric %u:%f\r', start(5), start(6)); 
    start = clock;
    t = transmission(image, al);
    start = clock - start;
    total = total + start;
    fprintf('transmission %u:%f\r', start(5), start(6)); 
    start = clock;
    r = scene_radiance(image, al, t);
    start = clock - start;
    total = total + start;
    fprintf('radiance %u:%f\r', start(5), start(6)); 
    fprintf('total %u:%f\r', total(5), total(6)); 
end