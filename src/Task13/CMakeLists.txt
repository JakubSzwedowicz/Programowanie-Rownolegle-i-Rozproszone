find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)

add_library(NelderMead STATIC nelderMead.c)
add_library(NelderMeadMPI STATIC nelderMeadMPI.c)
add_library(Common STATIC common.c)

target_compile_features(NelderMead PRIVATE c_std_17)
target_compile_features(NelderMeadMPI PRIVATE c_std_17)
target_compile_features(Common PRIVATE c_std_17)

target_include_directories(NelderMead PUBLIC .)
target_include_directories(NelderMeadMPI PUBLIC .)

target_link_libraries(Common PRIVATE m)
target_link_libraries(NelderMead PRIVATE Common OpenMP::OpenMP_C)
target_link_libraries(NelderMeadMPI PRIVATE Common OpenMP::OpenMP_C MPI::MPI_C)

add_subdirectory(Tests)
