cmake -G "MinGW Makefiles" -DSDL_STATIC=ON -DSDL_SHARED=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_DEBUG="-O3" ../CMakeLists.txt
mingw32-make
