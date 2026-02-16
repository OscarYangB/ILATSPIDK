cmake -G "MinGW Makefiles" -DSDL_STATIC=ON -DSDL_SHARED=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS_DEBUG="-g -O0" ../CMakeLists.txt
mingw32-make
::pause
