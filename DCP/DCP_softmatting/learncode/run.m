%% Single Image Haze Removal Using Dark Channel Prior - by Shiyu Dong and Yilin Yang
clear; close all; clc;

for i = 1 : 2  %遍历数据源。
    tic;%记录起始时刻
    %% read image
    try        
        img_file = ['../data/', num2str(i), '.jpg'];        
        img = imread(img_file);
    catch
        img_file = ['../data/', num2str(i), '.png'];        
        img = imread(img_file);
    end
    img = imresize(img, [500, NaN]);%调整图像尺寸大小。指定目标图像的宽度和高度，产生的图像有可能发生畸变。NaN:结果不定，not a number.

    %% dehaze
    [J, t_, t] = dehaze(img);
    imwrite(J, ['../results/', num2str(i), '_DarkChannel.png']);
    imwrite(t_, ['../results/', num2str(i), '_DarkChannel_TransRaw.png']);
    imwrite(t, ['../results/', num2str(i), '_DarkChannel_Trans.png']);
    time = toc;%根据前面tic，自动计算时�?   
    disp(['Image ', num2str(i), ' saved. Time: ', num2str(time), 's. ']);
end
