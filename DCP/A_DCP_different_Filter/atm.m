function atm = atm( img, JDark )
% Returning the atmospheric estimation

    width = size(img, 2);
    height = size(img, 1);
    
    pixels = width * height;
    
    % we need to take a small square of it.. Relative to overall size
    areaOfTests = ceil(pixels/1000); %$$$ may need to increase
    jDarkAreaVect = reshape(JDark, pixels, 1);
    realImageArea = reshape(img, pixels, 3);
    [jDarkAreaVect, ind] = sort(jDarkAreaVect);
    ind = ind(pixels - areaOfTests + 1 : end);
    
    atmosphere = zeros(1,3);
    for p = 1:areaOfTests
        test = ind(p);
        atmosphere = atmosphere + realImageArea(ind(p), :);
    end
    
    atm = atmosphere / areaOfTests;
    
end

