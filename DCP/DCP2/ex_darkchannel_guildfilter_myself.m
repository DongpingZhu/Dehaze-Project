clear

clc

close all

kenlRatio = .01;
minAtomsLight = 200;
omiga = .9;

image_name =  '5.bmp';
img=imread(image_name);
figure,imshow(uint8(img)), title('src');

sz=size(img);

w=sz(2);

h=sz(1);

dc = zeros(h,w);

for y=1:h

    for x=1:w

        dc(y,x) = min(img(y,x,:));

    end

end


% figure,imshow(uint8(dc)), title('Min(R,G,B)');

krnlsz = floor(max([3, w*kenlRatio, h*kenlRatio]))

dc2 = minfilt2(dc, [krnlsz,krnlsz]);

dc2(h,w)=0;

% figure,imshow(uint8(dc2)), title('After filter ');

dark_max = max(max(dc2));
if(length(sz)==3)
    img_hsv = rgb2hsv(img);
    img_v = img_hsv(:,:,3);
    [m,n] = find(img_v == max(img_v(find(dc2>=dark_max*0.98))));
    if(length(m)>1)
        m = m(1);
        n = n(1);
    end
    A(:,:,1) = min(double(img(m,n,1)),minAtomsLight);
    A(:,:,2) = min(double(img(m,n,2)),minAtomsLight); 
    A(:,:,3) = min(double(img(m,n,3)),minAtomsLight); 
else
    A(1,1) = min([minAtomsLight, max(max(dc2))]);
end
img_double = double(img);
dc_my = zeros(h,w);

if(length(sz)==3)
    for i = 1:h
        for j = 1:w
            dc_my(i,j) = min([img_double(i,j,1)/A(1,1,1),img_double(i,j,2)/A(1,1,2),img_double(i,j,3)/A(1,1,3)]);
        end
    end
else
    for i = 1:h
        for j = 1:w
            dc_my(i,j) = img_double(i,j)/A(1,1);
        end
    end
end
dc_my = minfilt2(dc_my, [krnlsz,krnlsz]);
dc_my(h,w)=0;
% figure,imshow(uint8(dc_my*255)),title('dc');

t = 1-omiga*dc_my;
t(find(t<0.25)) = 0.25;

% figure,imshow(uint8(t*255)),title('t');
t_d = t;

J = zeros(h,w,3);

img_d = double(img);
if(length(sz)==3)
    J(:,:,1) = (img_d(:,:,1) - A(1,1,1))./t_d + A(1,1,1);
    J(:,:,2) = (img_d(:,:,2) - A(1,1,2))./t_d + A(1,1,2);
    J(:,:,3) = (img_d(:,:,3) - A(1,1,3))./t_d + A(1,1,3);
else
    J = (img_d - A(1,1))./t_d + A(1,1);
end
figure,imshow(uint8(J),[]), title('结果1');

% % % % % % % % % % % % % % % % % % % % % % % % % % % % % 
r = krnlsz*4;
eps = 10^-6;
if(length(sz)==3)
filtered = guidedfilter_color(double(img)/255, t_d, r, eps);
else
    filtered = guidedfilter(double(img)/255, t_d, r, eps);
end
t_d = filtered;

% figure,imshow(t_d,[]),title('filtered t');

if(length(sz)==3)
    J(:,:,1) = (img_d(:,:,1) - A(1,1,1))./t_d + A(1,1,1);
    J(:,:,2) = (img_d(:,:,2) - A(1,1,2))./t_d + A(1,1,2);
    J(:,:,3) = (img_d(:,:,3) - A(1,1,3))./t_d + A(1,1,3);
else
    J = (img_d - A(1,1))./t_d + A(1,1);
end

figure,imshow(uint8(J)), title('最终结果');