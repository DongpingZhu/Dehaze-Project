function J = deHaze(im)

JDark = darkChannel(im);
A = atmLight(im, JDark);
transmission = transmissionEstimate(im, A);
J = getRadiance(A,im,transmission);