#!/bin/bash

mkdir -p work
cd work
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/lib
cmake --build . --config Release --target install
