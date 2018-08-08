% Input Options
%myfolder= '/Users/jixiang/Documents/MATLAB/a0134487x/';
%mysubfolder = 'Results/';
%outputPath = strcat(myfolder,mysubfolder);
outputPath = 'Results';
dataPath = 'Dataset/';
imageName = 'sample2.jpg';%sample1 - sample5

%%%%%%%%%%%%%%%%%%%%%%%


disp('please wait for around 30s');
%read image and convert it into double;
scalingFactor = 1;
im = imread(strcat(dataPath, imageName));
im = double(im);
im = im./255;

%(0)decrease the size of image for better excution proficiency.
imageRGB = imresize(im,scalingFactor);
imwrite(imageRGB,fullfile(outputPath,imageName));

%(1)Dark Channel Prior
dark = darkChannel(imageRGB);
imwrite(dark,fullfile(outputPath,strcat('DARK_',imageName)));

%(2)Estimating the Atmospheric Light
atmospheric = atmLight(imageRGB, dark);
imwrite(atmospheric,fullfile(outputPath,strcat('atmLight_',imageName)));

%(3)Estimating the Transmission
transmission = transmissionEstimate(imageRGB, atmospheric);
imwrite(transmission, fullfile(outputPath,strcat('TRANS_',imageName)));

%(4)Recovering the Scene Radiance
radiance = getRadiance(atmospheric, imageRGB, transmission);
imwrite(radiance, fullfile(outputPath,strcat('RAD_',imageName)));

%(5)Apply Levina's Soft Matting
refinedTransmission = matte(imageRGB, transmission);
imwrite(refinedTransmission, fullfile(outputPath,strcat('SOFT_TRANS_',imageName)));

 %(6)Recovering the Scene Radiance
refinedRadiance = getRadiance(atmospheric, imageRGB, refinedTransmission);
imwrite(refinedRadiance, fullfile(outputPath,strcat('RESULT_',imageName)));

disp('deHaze processing is finnished,please check Results folder');



   