% Show results
function resultsWithoutMat(im) 

%im = readIm(image);
J = deHazeWithoutMat(im);
figure;
%subplot(1,2,1);
%imagesc(im)
imshow(im)
title 'Original'
axis image off;
figure;
%subplot(1,2,2);
%imagesc(J)
imshow(J)
title 'De-hazed without applying soft matting algorithm';
axis image off;