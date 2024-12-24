#!/bin/bash

mkdir -p work
cd work
cmake -G Ninja .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/lib
cmake --build . -j10 --config Release --target install
