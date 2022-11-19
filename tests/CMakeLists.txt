add_executable(CAVE_TESTS_WRITER writer-test.c)
add_executable(CAVE_TESTS_POLYTRI polytri-test.c)

target_link_libraries(CAVE_TESTS_WRITER CAVE)
target_link_libraries(CAVE_TESTS_POLYTRI CAVE)

#copies the assests to the build directory such that we have "buildDir/tests/assets/"
file(COPY assets DESTINATION ./)