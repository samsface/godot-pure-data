cmake_minimum_required(VERSION 3.10)
project(gdnative-example)

###############################################################################
# options
###############################################################################

# build universal binaries on macos
if(APPLE)
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
endif()

###############################################################################
# libs
###############################################################################

# libpd requires pthreads so using this windows port downloaded from the
# official site of the win pthread project
if (MSVC)
  set(PTHREADS_LIB "${CMAKE_CURRENT_LIST_DIR}/lib/pthreads/lib/pthreadVC2.lib" CACHE PATH "..." FORCE)
  set(PTHREADS_INCLUDE_DIR  "${CMAKE_CURRENT_LIST_DIR}/lib/pthreads/include" CACHE PATH "..." FORCE)
endif()

add_subdirectory(lib/godot-cpp)
add_subdirectory(lib/libpd)

###############################################################################

# install build into example project by default
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(
    CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/project/addons/pure-data/bin"
    CACHE PATH "..."
    FORCE)
endif()

# let's us link depended dlls relative to process on linux
set(CMAKE_INSTALL_RPATH "$ORIGIN")

add_library(pd-godot SHARED src/pure_data_audio_stream_player.cpp src/register_types.cpp)

target_link_libraries(
  pd-godot
  PRIVATE
    godot-cpp
    libpd_static
    "$<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_LIST_DIR}/lib/pthreads/lib/pthreadVC2.lib>"
    "$<$<PLATFORM_ID:Windows>:Ws2_32.lib>"
)

set_target_properties(
  pd-godot
  PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON)

install(TARGETS pd-godot DESTINATION .)
