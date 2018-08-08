%This is project entry1 for display result directly
% Input Options
dataPath = 'Dataset/';
imageName = 'sample2.jpg';%from sample1 - sample5


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%read the image
im = imread(strcat(dataPath, imageName));

%decrease ImageSize to increase execution effiency.
scalingFactor =1;
im = imresize(im, scalingFactor);%adjust the size of image,1 is original
%convert image into double precision.
im = double(im);
im = im./255;
%Display the result into subplot.
%results(im);
resultsWithoutMat(im);