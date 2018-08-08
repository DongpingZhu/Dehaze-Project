function transmission = transmissionEstimate(im, A)

%In real life, a little amount of haze will always be around us.
%If we don't transmit,the image will be really sharp which is not
%realistic.

omega = 0.95; % So the amount of 5% haze we're keeping

im3 = zeros(size(im));
for ind = 1:3 
    im3(:,:,ind) = im(:,:,ind)./A(ind);
end

% imagesc(im3./(max(max(max(im3))))); colormap gray; axis off image

transmission = 1-omega*darkChannel(im3);
