function [ medianImg ] = customMedianFilter( W )
    [h, w] = size(W);
    B = medfilt2(W);    % matlab 3x3 median filter
        
    %%% go through every pixel.    
    diff = imabsdiff(W, B); %% for contrast boost
    C = imsubtract(B, diff);
    medianImg = zeros(h, w);
    for i = 1:w
        for j = 1:h
            minB = min(C(j,i),W(j,i));
            medianImg(j,i) = max(minB,0);
        end
    end
end

