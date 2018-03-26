cd Src\Matlab_Backend\Dependencies\quickscore
try 
    mex  COMPFLAGS="$COMPFLAGS /Ox /openmp /W3 /favor:INTEL64" -I. quickscore.cpp image.cpp patch.cpp rotate.cpp score.cpp
catch E
    warning('Could not compile quickscore')
    warning(E)
end

cd ..\..\..\..\

%-g  /favor:INTEL64 /DMATLAB
%/Ox /W3 /openmp


