Program name:
  Dehazing

Author:
  Jin Hwan Kim

  Media Communications Laboratory, 
  School of Electrical Engineering,
  Korea University, Seoul, Korea
  

  E-mail: arite AT korea.ac.kr

License:
  The source files in ./src/ directory
  
  The guided filter is He et al.'s guided image filtering, and we implement in C.
  
  The other part will be useful but WITHOUT ANY WARRANTY; 
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

  Permission is granted for research and educational use.
 
  If you want to include this software in a commercial product, please contact 
  the author. 

Citation:
  J.-H. Kim, W.-D. Jang, Y. Park, D.-H. Lee, J.-Y. Sim, C.-S. Kim, "Temporally
  coherent real-time video dehazing," in Proc. IEEE ICIP, 2012.
  J.-H. Kim, W.-D. Jang, J.-Y. Sim, C.-S. Kim, "Optimized Contrast Enhancement 
  for Real-Time Image and Video Dehazing," J. Vis. Commun. Image R. Vol. 24, No.3,  pp.410-425, Apr, 2013.

Install:
  The program was tested using 
  Visual studio 2010 with windows 7
  OpenCV 2.3.1 is required

Usage:
  video-
	dehazing input_filename output_filename [frames]
  image-
	dehazing input_filename output_filename

  you may modify the some parameters in dehazing constructor or modify the code to add
  parameters to excutable file.

Example: 
  dehazing "sample\cross.avi" "outputs\cross_out.avi" 100
  
Output:
  programe makes an outfile, which is dehazed.

Version History:
  
  1.0   12-Mar-2013    Initial Release