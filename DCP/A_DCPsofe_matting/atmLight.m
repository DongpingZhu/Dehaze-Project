function A = atmLight(im, JDark)

% the color of the atmospheric light is very close to the color of the sky
% so just pick the first few pixels that are closest to 1 in JDark
% and take the average

% pick top 0.1% brightest pixels in the dark channel

% get the image size
[height, width, ~] = size(im);
imsize = width * height;

numpx = floor(imsize/1000); % accomodate for small images
%disp(numpx);
JDarkVec = reshape(JDark,imsize,1); % a vector of pixels in JDark
ImVec = reshape(im,imsize,3);  % a vector of pixels in my image

[JDarkVec, indices] = sort(JDarkVec); %sort
indices = indices(imsize-numpx+1:end); % need the last few pixels because those are closest to 1


%find the first 10 lines of pixel, because it is somehow could be the sky.
for j = 1:10
    for i = 1:width
        [maxVal,maxInd] = max(JDark(:));
    end
end

%find the last few pixels
atmSum = zeros(1,3);
for ind = 1:numpx
    atmSum = atmSum + ImVec(indices(ind),:);
end

possibleResult1 = atmSum / numpx;
possibleResult2 = [maxVal,maxVal,maxVal];

%Between two possible results, we pick the brightest one(which is closer to
%1)
if possibleResult1 > maxVal
    A = possibleResult1;
else
    A = possibleResult2;
end


