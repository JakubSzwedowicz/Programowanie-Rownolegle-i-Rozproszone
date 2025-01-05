add_library(Utils utils.c)
target_compile_features(Utils PRIVATE c_std_17)
target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_library(Logger logger.c)
target_compile_features(Logger PRIVATE c_std_17)
target_include_directories(Logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})