cmake_minimum_required(VERSION 3.19)

project(Programowanie_Rownolegle_I_Rozproszone LANGUAGES C)

add_compile_options(-O3)

add_subdirectory(src)

# Target for OpenMP and MPI
find_package(MPI REQUIRED)
add_executable(appMPI mainMPI.c)
target_compile_features(appMPI PRIVATE c_std_17)
target_compile_options(appMPI PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(appMPI PRIVATE NelderMeadMPI m Utils Logger MPI::MPI_C)

# Target for OpenMP only
add_executable(app main.c)
target_compile_features(app PRIVATE c_std_17)
target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(app PRIVATE NelderMead m Utils Logger)
