# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 2.6)
find_package(Rock)
include(RockRuby)
rock_activate_cxx11()
rock_init(visual_detectors 0.1)
rock_standard_layout()
