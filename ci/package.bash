#!/bin/bash

cp LICENSE project/addons/pure-data/

cp $1/*/lib/*.so    project/addons/pure-data/bin/
cp $1/*/lib/*.dll   project/addons/pure-data/bin/
cp $1/*/lib/*.dylib project/addons/pure-data/bin/
