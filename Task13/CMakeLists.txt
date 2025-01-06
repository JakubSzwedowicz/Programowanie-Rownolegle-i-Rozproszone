find_package(OpenMP REQUIRED)

add_library(NelderMead STATIC nelderMead.c)

target_compile_features(NelderMead PRIVATE c_std_17)

target_include_directories(NelderMead PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(NelderMead PRIVATE m OpenMP::OpenMP_C)

add_subdirectory(Tests)
