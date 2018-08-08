clear;
close all;
img=imread('2.bmp');
G = fspecial('gaussian', [50 50], 2);
Ig = imfilter(img,G);
imshow(Ig);