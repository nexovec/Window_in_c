link_libraries(glfw glad)
include_directories("${GLFW_SOURCE_DIR}/include")
include_directories("${HELLO_SOURCE_DIR}/vendor/glad/include")
include_directories("${HELLO_SOURCE_DIR}/src")
file(GLOB_RECURSE SOURCES RELATIVE ${HELLO_SOURCE_DIR}/src "*.c" "*.h")
file(GLOB_RECURSE SOURCES_T RELATIVE ${HELLO_SOURCE_DIR}/src "../tests/*.c" "../tests/*.h")
add_executable (App ${SOURCES} ${SOURCES_T})
add_custom_command(TARGET App PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:App>/res)