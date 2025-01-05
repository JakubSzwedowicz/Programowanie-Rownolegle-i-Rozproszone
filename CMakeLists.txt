cmake_minimum_required(VERSION 3.19)

project(Programowanie_Rownolegle_I_Rozproszone LANGUAGES C)

add_executable(app main.c utils.c)
target_compile_features(app PRIVATE c_std_17)
target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)

add_subdirectory(Task13)

target_link_libraries(app PRIVATE NelderMead m)

