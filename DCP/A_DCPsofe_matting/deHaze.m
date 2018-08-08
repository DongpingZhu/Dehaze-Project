function J = deHaze(im)

JDark = darkChannel(im);
A = atmLight(im, JDark);
transmission = transmissionEstimate(im, A);
refinedTransmission = matte(im, transmission);
J = getRadiance(A,im,refinedTransmission);