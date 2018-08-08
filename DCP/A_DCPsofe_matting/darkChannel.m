function JDark = darkChannel(im2)

[height, width, ~] = size(im2);

%the patch size is set to be 10 x 10 which could be more accurate than
%original parameter
patchSize = 15; 
% half the patch size to pad the image with for the array
padSize = 7;

JDark = zeros(height, width); % initialize the dark channel matrix
imJ = padarray(im2, [padSize padSize], Inf); % create the new image by adding 10x10 patches
%imagesc(imJ); colormap gray; axis off image
%printmat(imJ);

for j = 1:height
    for i = 1:width
        % the patch has top left corner at (jj, ii)
        patch = imJ(j:(j+patchSize-1), i:(i+patchSize-1),:);
        % the dark channel for a patch is the minimum value for all
        % channels for that patch
        JDark(j,i) = min(patch(:));
     end
end
