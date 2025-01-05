add_executable(tests testNelderMead.c)

target_compile_features(tests PRIVATE c_std_17)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(tests PRIVATE NelderMead)