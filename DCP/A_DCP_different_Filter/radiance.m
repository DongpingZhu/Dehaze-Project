function r = radiance(A, img, t)
% recovers scene radiance from the picture
    t0 = .1;
    newImg = zeros(size(img));
    for i = 1:3
        newImg(:,:,i) = A(i) + (img(:,:,i) - A(i))./max(t,t0);
    end
    r = newImg./(max(max(max(newImg))));
end

