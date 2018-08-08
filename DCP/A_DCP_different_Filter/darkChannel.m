function jdark = darkChannel(img)
% computes the jdark channel of the image
    height = size(img, 1);
    width = size(img, 2);
        
    sizeOfArea = 15;
    pad = 7; % may not need
    
    jdark = zeros(height, width);
    newImage = padarray(img, [pad pad], Inf);    
    for j = 1:height
        for i = 1:width
            patch = newImage(j:(j+sizeOfArea-1), i:(i+sizeOfArea-1), :);                        
            jdark(j, i) = min(patch(:));
        end
    end
           
    
end