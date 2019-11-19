# Open The CMake GUI
# specify the source directory and the binary directory
# press configure
# set CMAKE_INSTALL_PREFIX to the path where you want to install the program
# press configure
#
# UNIX Makefile:
# 1) go to the binary folder and type "make" to build the project
# 2) (optional) type "make install all" to install the files into the install
#    directory
#
# Microsoft Visual C++:
# 1) open the .sln file
# 2) change the mode to "Release" -> only Release is supported!
# 3) build the project "ALL_BUILD" to build the opentld project
# 4) build the project "INSTALL" to install the files into the install
#    directory
# 5) build the project "PACKAGE" to create an NSIS-installer (NSIS is required)


cmake_minimum_required(VERSION 2.8)
project(stereovision)
find_package(qibuild)

add_subdirectory(src/3rdparty/cvblobs)
add_subdirectory(src/libopentld)
add_subdirectory(src)
