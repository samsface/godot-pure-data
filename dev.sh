#!/bin/bash

mkdir -p work
cd work
cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/lib
cmake --build . -j10 --config Release --target install

mkdir -p ../project/addons/pure-data/bin
cp install/lib/*.so    ../project/addons/pure-data/bin/
cp install/lib/*.dll   ../project/addons/pure-data/bin/
cp install/lib/*.dylib ../project/addons/pure-data/bin/
