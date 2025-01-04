cmake_minimum_required(VERSION 3.19)

project(Programowanie_Rownolegle_I_Rozproszone LANGUAGES C)

add_executable(App main.c utils.c)
target_compile_features(App PRIVATE c_std_17)

add_subdirectory(Task13)
target_link_libraries(App PRIVATE Nelder-Mead m)

