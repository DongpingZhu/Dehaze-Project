%% step 2: transmission - by Shiyu Dong
function t_ = Transmission(img, A, patch_size, omega)

% Normalize 3 channels of img according to A
norm_img = double(img)./A;

% dark channel
dark_channel = CalcDarkChannel(norm_img, patch_size);%相当于对norm_img进行取暗通道操作，即取两次最小值。

% t_
t_ = 1 - omega*dark_channel;%对公式的实现。


end