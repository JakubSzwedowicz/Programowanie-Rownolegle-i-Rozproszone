find_package(MPI REQUIRED)
add_executable(tests testNelderMead.c)
add_executable(testsMPI testNelderMeadMPI.c)

target_compile_features(tests PRIVATE c_std_17)
target_compile_features(testsMPI PRIVATE c_std_17)

target_include_directories(tests PRIVATE .)
target_include_directories(testsMPI PRIVATE .)

target_link_libraries(tests PRIVATE NelderMead)
target_link_libraries(testsMPI PRIVATE NelderMeadMPI MPI::MPI_C)