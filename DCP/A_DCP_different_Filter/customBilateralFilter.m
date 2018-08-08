function [ r ] = customBilateralFilter( W )
%using a predefined 3x3 patch with distance and intensity filters

    [h, w] = size(W);
    r = zeros(h, w);
    dist = [
         sqrt(2) 1 sqrt(2);
         1 0 1;
         sqrt(2) 1 sqrt(2)
        ];
    for i = 2:w - 1
        for j = 2:h - 1
            patch = [
                     W(j-1,i-1) W(j-1,i) W(j-1,i+1);
                     W(j,i-1) 0 W(j,i+1);
                     W(j+1,i-1) W(j+1,i) W(j+1,i+1) 
                    ];            
            top = 0;
            bottom = 0;
            for z = 1:9
                top = top + dist(z) * abs((W(j,i) - patch(z))) * patch(z);
                bottom = bottom + dist(z) * abs((W(j,i) - patch(z)));
            end
            result = top / bottom;
            if isnan(result)
                r(j,i)= W(j,i);            
            else
                r(j,i)=result;
            end
        end
    end

end

