function [ guidedRefinedT ] = guidedBilateral(t, I)
% lets run our own custom kernel filter over the top
% of our existing transmission estimate
% the kernel was inspired from the Kaiming He paper
    
    height = size(I, 1);
    width = size(I, 2);
    guidedRefinedT = zeros(height, width);
    
    % hard coded bilateral vals.
    sigma1 = height * width;
    w  = 6; % 6 x 6 window around!
    
    %storage vals
    I  =  im2double(I);                
    f  =  jbfilter2(t, mat2gray(I), w,sigma1);

    
    % custom bilateral filter
%     kernel = 0;
%     innerSum = 0;
%     for i = 1: w
%         for j = 1:w
%             
%         
%         end
%     end
%     iteration = (1/w^2) * (innerSum);
%     
%     
%     
%     for i = 1:width
%         for j = 1: height
%             sum = 0;
%             for k = 1: height
% 
%                 sum = sum + (f(k,i) * t(k,i));                
%             end
%             guidedRefinedT(j, i) = sum;
%         end
%     end
%     guidedRefinedT = mat2gray(guidedRefinedT);
    guidedRefinedT = mat2gray(f);


end

