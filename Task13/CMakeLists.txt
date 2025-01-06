find_package(OpenMP REQUIRED)

add_library(NelderMead STATIC nelderMead.c)
add_library(Common STATIC common.c)

target_compile_features(NelderMead PRIVATE c_std_17)
target_compile_features(Common PRIVATE c_std_17)

target_include_directories(NelderMead PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Common PRIVATE m)
target_link_libraries(NelderMead PRIVATE Common OpenMP::OpenMP_C)

add_subdirectory(Tests)
