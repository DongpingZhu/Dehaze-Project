function trans = trans(VR, A)
% gets the transmission of th image
    [h, w] = size(VR);
    omega = .95; %haze kept.. Could boost
    trans = zeros(size(VR));        
    
    for i = 1:w
        for j = 1:h
            trans(j,i) = 1 - ((omega*VR(j,i))/A);
        end
    end 
    %trans = 1-omega*darkChannel(newImg);
end

