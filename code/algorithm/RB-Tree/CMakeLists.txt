cmake_minimum_required(VERSION 3.10)
project(RB_Tree)

set(CMAKE_CXX_STANDARD 11)

add_executable(RB_Tree main.cpp Utils.cpp Utils.h Node.cpp Node.h RedBlackTree.cpp RedBlackTree.h)