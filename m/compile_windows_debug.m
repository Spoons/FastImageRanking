cd Src\Matlab_Backend\Util\quickscore
try 
    mex  -v COMPFLAGS="$COMPFLAGS /DMATLAB /openmp /W3 /favor:INTEL64" -I. -g quickscore.cpp image.cpp patch.cpp rotate.cpp score.cpp
catch E
    warning('Could not compile quickscore')
    fprrintf(E);
end

cd ..\..\..\..\

%-g  /favor:INTEL64 /DMATLAB
%/Ox /W3 /openmp
