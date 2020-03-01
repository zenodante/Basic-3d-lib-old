cmake_minimum_required(VERSION 3.1)
project (3dlib)
include (../../32blit.cmake)
blit_executable (3dlib example_ship.cpp cat.c b3dlib.c b3L_support.c ship2_72tri.c)
