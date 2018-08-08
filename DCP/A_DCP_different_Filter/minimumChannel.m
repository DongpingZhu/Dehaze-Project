function [ B ] = minimumChannel( I )
%BILATERAL Summary of this function goes here
% Computes the minimum channel of the input image
% It's as simple as looking at channel values for each pixel, and taking
% the minimum value

[height, width, depth] = size(I);

B = zeros(height, width);

for i=1 : height
    for j=1 : width
        temp1 = I(i,j,1);
        temp2 = I(i,j,2);
        temp3 = I(i,j,3);
        B(i,j) = min(I(i,j,1:3));
    end
end

end

