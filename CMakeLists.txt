cmake_minimum_required(VERSION 3.10)

project(SeaShell VERSION 1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(seashell main.c headers/sea_shell.c) 
target_include_directories(seashell PRIVATE headers)
