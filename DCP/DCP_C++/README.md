原文：[博客文章地址](http://loliko.me/single-image-haze-removal-using-dark-channel-prior/)

理论上来说代码一定是能编译能过的。
用的opencv库版本有2.89和3.10  


#前言
目前的图像去雾算法很多，思路基本上两条：   
* 1,基于非物理模型，本质上是增强图像的对比度与颜色，并没有对雾天图像的形成原因进行分析而补偿。代表方法是直方图均衡化。效果一般。  
* 2，基于物理模型，现在效果好的去雾算法都是基于物理模型，利用大气物理散射规律来建立图像还原模型，而不同的论文算法，用的模型都不尽相同。基于物理模型的算法因为基于模型更好的分析了含雾图像。并且与现实贴近，效果都不错，只是算法复杂度不同，计算时间长短不同而已。代表方法是何恺明博士的[Single Image Haze Removal Using Dark Channel Prior][1]，即基于暗通道先验的去雾算法。
目前感觉效果最好的就是基于暗通道先验的去雾算法。  

##Single Image Haze Removal Using Dark Channel Prior
在去雾算法中，利用的以下物理模型：![此处输入图片的描述][2]  
其中I(x)是有雾后的图像，J(x)是无雾图像，A是全球大气光照值，t(x)是透射率图。

上式经过化简之后可得到：![][3]  
其实就是已知I(x)，然后通过分析I(x)，算出J(x)。

##暗通道
首先看什么是暗通道，在何的论文中，对于暗通道的定义是：在无雾图像中，在大多数局部区域内，其中的一些像素会在某个通道内含有非常低的像素值(换句话说也就是，在某个区域内，所有像素的各个通道的最小值的像素值非常小（0~16）)。这些像素值的产生主要是由于阴影（shadow）, 彩色物体（colorful object）（某一个通道的值太大，导致其他通道的值小）, 黑色物体等。  
而在含雾图像中，因为雾气的存在，暗通道并不是全黑，比如下图：
![][4]
可以看出来，上方含雾的部分偏白，而下方城市部分大部分为黑色。这样我们都能得用暗通道来得到含雾图像中雾的有无，雾的浓度。这个是作者分析了很多图像后得出来的结论，所以说讲先验。 
 
暗通道的计算方法也很容易  
![][5]

其中Ω为一个正方形区域，即对每一个像素计算每一个通道一个区域内最小值的那个像素值作为暗通道的值。

##全球大气光照值
因为在计算透射率图的时候要用到全球大气光照值，所以先计算A。
A的计算方法很简单，

> the pixels with highest intensity in the input image I is selected as the atmopheric light.

这是文中给出的方法，在暗通道中取亮度最高的前0.1%像素，然后找出在原始图像中I亮度最高的像素作为A。

##透射率
对于透射率的计算有以下式子：
![][6]

其中，w是为了不使图像失真，而引人的控制保留雾的比重的参数。  
如果只是这么做了话，因为透射率图的计算方法，我们是在一个个区域内算的，所以透射率图会是一块块的。所以我们还要对透射率图进行处理。计算出精细的透射率图。  
在原论文中，采用的软扣图算法，十分慢。  

##去雾
我们现在已经计算得出了A，t(x)，I(x)，可以进行去雾了。  

如下式：
![][7]  

其中t0是为了防止t(x)中的值为0以及防止t(x)过下而使J(x)过大，J(x)过大会使图像偏白。  

#改进
* 原算法中，计算精细的透射率图是采用的软扣图，而我改用了同是何博士发的算法论文中的[导向滤波][8]。而导向滤波是一个快速的滤波算法，感兴趣的可以看看论文。而现在MATLAB与OpenCV中集成了此滤波算法。在我写的时候还没有，所以自己写了一个导向滤波。其实何博士的论文对于导向滤波讲的很清楚，推荐大家去读一读。  
* 算法对于天空部分计算不是很好，特别是还有雾的天空，会使天空出现明显的不自然。  
这里我采用了一种很简单的方法来改进：  
引入一个参数K，如果全球大气光照值A与对应的暗通道值的绝对值小于K就对除以这个绝对值并乘以K。如果大于的话就不变。

改进前：
![][9]

改进后：
![][10]

效果很明显。 

可以在最后计算式中加上一个小参数来增加图像的亮度。因为去雾后的图像会速度偏暗。何的论文中的图像都经过了再次处理。 

A的值可以取3个通道的平均值，也可以取前0.1%的平均值。也可以两者都取。 

在计算导向滤波的时间可以用原图，也可以用原图的灰度图。不过用原图的话，因为是三个通道都要计算，所以会比单通道要慢一点。  

其实还有一些改进的地方但是都是一些可说可不说的，感觉大家都讲的差不多了。  

对了，何博士好像不久前又写了一篇论文，是对导向滤波的改进,叫快速导向滤波。感叹一下，又会有人水一篇论文了：用快速导向滤波来做去雾。  

##效果
![][11]![][12]  

![][13]

![][14]  

![][15]

##感想
滤波算法还有不少。但是基本上都出不了这个框架了。并且我也看不了不少论文，自己也写了一些，感觉就这个算法比较稳定，优化优化，对于任何图像都有不错的效果，并且速度快，能用于视频处理。

其实现在国内很多文章都是在优化这个算法，点子基本上都是放在了透射率图的优化上，计算速度，去雾效果等等，比如用其它的滤波方法，对t(x)的值进行限定，等等。各种方法都有，效果都还不错，只是速度应该快不起来，这就是学术与工业的差别。  

我在参考另外一个博客的时候，发现他写了一个时间复杂度为O(1)的去雾算法，我也看了看，感觉效果没有这个好，虽然速度快。具体看：[一种可实时处理 O(1)复杂度图像去雾算法的实现][16]。  

另外一个效果比较好的，那位大神也写了：[优化的对比度增强算法用于有雾图像的清晰化处理][17]。  

##感谢以及参考
感谢Imageshop，写出了很多图像处理方面的好文章。自己去雾方面就是在这里入的门。非常感谢他的无私奉献。  
感谢何博士为我国贡献了第一篇CVPR Best Paper。**这才是认真做学术的人，而不是一个骗骗钱的教授！**  

参考：

[基于暗通道的图像去雾算法_CVPR09 BestPaper_KaiMing He][18]  
[《Single Image Haze Removal Using Dark Channel Prior》一文中图像去雾算法的原理、实现、效果（速度可实时）][19]  
[opencv实现导向滤波][20]  
[Single Image Haze Removal][21]  
[Guided Image Filtering][22]  


  [1]: http://research.microsoft.com/en-us/um/people/kahe/cvpr09/index.html
  [2]: http://7nj2fn.com1.z0.glb.clouddn.com/3.png
  [3]: http://7nj2fn.com1.z0.glb.clouddn.com/4.png
  [4]: http://7nj2fn.com1.z0.glb.clouddn.com/1.png
  [5]: http://7nj2fn.com1.z0.glb.clouddn.com/2.jpg
  [6]: http://7nj2fn.com1.z0.glb.clouddn.com/6.jpg
  [7]: http://7nj2fn.com1.z0.glb.clouddn.com/4.png
  [8]: http://research.microsoft.com/en-us/um/people/kahe/eccv10/index.html
  [9]: http://7nj2fn.com1.z0.glb.clouddn.com/QQ%E6%88%AA%E5%9B%BE20150705210753.png
  [10]: http://7nj2fn.com1.z0.glb.clouddn.com/QQ%E6%88%AA%E5%9B%BE20150705210611.png
  [11]: http://7nj2fn.com1.z0.glb.clouddn.com/QQ%E6%88%AA%E5%9B%BE20150705213756.png
  [12]: http://7nj2fn.com1.z0.glb.clouddn.com/QQ%E6%88%AA%E5%9B%BE20150705213801.png
  [13]: http://7nj2fn.com1.z0.glb.clouddn.com/QQ%E6%88%AA%E5%9B%BE20150705214035.png
  [14]: http://7nj2fn.com1.z0.glb.clouddn.com/QQ%E6%88%AA%E5%9B%BE20150705214146.png
  [15]: http://7nj2fn.com1.z0.glb.clouddn.com/QQ%E6%88%AA%E5%9B%BE20150705214319.png
  [16]: http://www.cnblogs.com/Imageshop/p/3410279.html
  [17]: http://www.cnblogs.com/Imageshop/p/3925461.html
  [18]: http://www.cnblogs.com/ztfei/archive/2012/09/02/2667607.html
  [19]: http://www.cnblogs.com/Imageshop/p/3281703.html
  [20]: http://blog.csdn.net/wds555/article/details/23176313
  [21]: http://research.microsoft.com/en-us/um/people/kahe/cvpr09/index.html
  [22]: http://research.microsoft.com/en-us/um/people/kahe/eccv10/index.html