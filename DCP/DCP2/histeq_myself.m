close all
clc
rgb=imread('0.jpg');
% �Ա��Ͷ������Ƚ���histeq����
hsv=rgb2hsv(rgb);
h=hsv(:,:,1);
s=hsv(:,:,2);
v=hsv(:,:,3);
% H = histeq(h);
S=histeq(s);
V=histeq(v);
result_hsv=hsv2rgb(h,S,V);

% ��RGBÿ��ͨ������histeq����
r=rgb(:,:,1);
g=rgb(:,:,2);
b=rgb(:,:,3);
R=histeq(r);
G=histeq(g);
B=histeq(b);
result_rgb=cat(3,R,G,B);

% ��YCbCr�����Ƚ���histeq����
ycbcr=rgb2ycbcr(rgb);
y=ycbcr(:,:,1);
cb=ycbcr(:,:,2);
cr=ycbcr(:,:,3);
Y=histeq(y);
result_ycbcr=ycbcr2rgb(cat(3,Y,cb,cr));

% �����ʾ
figure,imshow(rgb),title('ԭʼͼ��')
figure,imshow(result_hsv),title('HSV')
figure,imshow(result_rgb),title('RGB')
figure,imshow(result_ycbcr),title('YCbCr')