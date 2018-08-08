function [t] = transmission(image, atmospheric_light)
    cols = size(image, 2);
    rows = size(image, 1);
    t = zeros(rows, cols);
    w = .95;
    for ix = 8:rows-8
        for iy = 8:cols-8
            t(ix-7:ix+7, iy-7:iy+7) = calculate_transmission(image(ix-7:ix+7, iy-7:iy+7), atmospheric_light, w);
        end
    end
end