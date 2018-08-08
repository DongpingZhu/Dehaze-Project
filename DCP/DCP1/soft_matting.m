function [t] = soft_matting(transmission, image)
    cols = size(image, 2);
    rows = size(image, 1);
    mu_k = mean2(image);
    sigma_k = cov(image);
    wk = cols * rows;
    k_delta = 0;
    u3 = eye(3);
    t = 0; 
    L = (k_delta - (
end