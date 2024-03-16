#!/bin/bash

mkdir -p work
cd work
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../addons/pure-data
cmake --build . --target install
