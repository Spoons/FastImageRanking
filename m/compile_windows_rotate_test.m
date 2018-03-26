
    mex  -v COMPFLAGS="$COMPFLAGS /DMATLAB /DDEBUG /openmp /W3 /favor:INTEL64" -I. -g testing/rotate_image.cpp score.cpp image.cpp patch.cpp rotate.cpp

%-g  /favor:INTEL64 /DMATLAB
%/Ox /W3 /openmp
