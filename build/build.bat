::cmake -G "MinGW Makefiles" -DSDL_STATIC=ON -DSDL_SHARED=OFF -DCMAKE_BUILD_TYPE=Debug -DFILE_EMBED=ON ../CMakeLists.txt
mingw32-make -j 32
