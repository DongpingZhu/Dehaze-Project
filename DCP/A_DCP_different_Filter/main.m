% main file to organize code from.
clear;

% TO RUN THIS CODE, REPLACE 3.JPG with an input image. 
% subsequent image write names don't need to be changed to view results,
% but it is reccommended
%read input (hazey)s
regImg = im2double(imread('results/3.JPG')); %im2double(imread('1.JPG'));
minImg = regImg; %will eventually be "minimum channel image"
%downsampling
downImg = minImg; %imresize(minImg,0.5,'box');
%imshow(downImg);
[h, w] = size(downImg);

% minimum channel image
W = minimumChannel(downImg);
imwrite(W, 'results/3.W.jpg', 'jpg');
imshow(W);


% median filter to use with the bilateral filtered pic
% later on with the guided Joint Bilateral Filter
medianResult = customMedianFilter(W);
imwrite(medianResult, 'results/3.medianResult.jpg', 'jpg');
imshow(medianResult);


% bilateral filter. First applied filter
% jbfilter2 is a joint bilateral filter, but degrades
% to a normal bilateral filter by passing in the image as its own guide
%bilateralResult = jbfilter2(M, M, 6, 1000);
bilateralResult = customBilateralFilter(W);
imwrite(bilateralResult, 'results/3.bilateralResult.jpg', 'jpg');
imshow(bilateralResult);


% guided joint bilater filte. Second/Final filter
% custom filter function based on the Xiao paper
%G = guidedJointBilateralFilter(medianResult, bilateralResult);
G = jbfilter2(medianResult, bilateralResult, 6, 1000);
imwrite(G, 'results/3.jbfilter.jpg', 'jpg');
imshow(G);



% atmoshpheric light (A) (uses regular img **fast enough)
% dark channel prior (using downsampled minimum channel image)
jdark = darkChannel(W);
imwrite(jdark, 'results/3.jdark.jpg', 'jpg');
A = atm(downImg, jdark);

% transmission estimation.. non-difracted Light essentially
t = trans(G, max(A)); %use highest value of A to estimate Atmosphere
imwrite(t, 'results/3.transmission.jpg', 'jpg');
imshow(t);

%refine using guided filter 
%refineT = guidedBilateral(t, downImg); % t is guided by downImg
%imshow(refineT);

% radiance method without refinement
r = radiance(A, regImg, t);
imwrite(r, 'results/3.radiance.jpg', 'jpg');

%test results
imshow(r)
%imshow(downImg);

%{
final = imadjust(r,[0, .7],[.1, 1]);
imshow(final);
imshow(regImg);
%}

%{
% fast matting
%rtf = fastMatte(img, t, A);

% soft matting
%rt = matte(img, t);
%}



% final image -- de(hazed)
%imwrite(downImg, 'guidedTest.jpg', 'jpg');
%imwrite(r, 'end.jpg', 'jpg');
%imwrite(final, 'guidedTestResultsFilter.jpg', 'jpg');
