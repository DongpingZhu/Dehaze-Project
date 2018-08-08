function [transmission] = calculate_transmission(patch, atmospheric_light, w)
    [value ~] = find_minimum(patch/atmospheric_light);
    transmission = 1 - w * value;
end