add_library(Nelder-Mead STATIC Nelder-Mead.c)

target_compile_features(Nelder-Mead PRIVATE c_std_17)

target_include_directories(Nelder-Mead PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Nelder-Mead PRIVATE m)