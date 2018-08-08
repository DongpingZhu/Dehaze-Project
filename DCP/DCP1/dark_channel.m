function [dark_channel] = dark_channel(image)
    cols = size(image, 2);
    rows = size(image, 1);
    
    dark_channel = zeros(rows, cols);
    for ix = 8:rows-8
        for iy = 8:cols-8
            dark_channel(ix-7:ix+7, iy-7:iy+7) = find_minimum(image(ix-7:ix+7, iy-7:iy+7));
        end
    end
end