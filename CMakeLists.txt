cmake_minimum_required(VERSION 3.10)
project(gdnative-example)

###############################################################################
# options
###############################################################################

OPTION(BUILD_SHARED_LIBS "Build shared libraries" ON)


###############################################################################
# libs
###############################################################################

add_subdirectory(lib/godot-cpp)
add_subdirectory(lib/libpd)

###############################################################################

# install build into example project by default
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(
    CMAKE_INSTALL_PREFIX "../../addons/pure-data" 
    CACHE PATH "..." 
    FORCE)
endif()

# let's us link relative to process on linux
set(CMAKE_INSTALL_RPATH "$ORIGIN")

add_library(pd-godot src/gdexample.cpp src/register_types.cpp)

target_link_libraries(
  pd-godot
  PRIVATE
    godot-cpp
    libpd)

set_target_properties(
  pd-godot
  PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)

if(APPLE)
  set_target_properties(
    pd-godot
    PROPERTIES 
    OSX_ARCHITECTURES "arm64;x86_64")
endif()

install(TARGETS pd-godot DESTINATION .)

