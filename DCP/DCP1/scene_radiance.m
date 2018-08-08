function [r] = scene_radiance(image, atmospheric_light, transmission)
    cols = size(image, 2);
    rows = size(image, 1);
    r = zeros(rows, cols, 3);
    t(:,:,1) = transmission;
    for ic = 1:3
        for ix = 1:rows
            for iy = 1:cols
                r(ix, iy, ic) = (image(ix, iy, ic) - atmospheric_light) / (max(t(ix, iy), .1)) + atmospheric_light;
            end
        end
    end
end