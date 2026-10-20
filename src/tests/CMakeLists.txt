set(NAME FpsAimForgeTests)

set(TEST_SOURCES
  "aim_db_test.cc"
  "backup_test.cc"
  "bundle_functional_test.cc"
  "geometry_test.cc"
  "search_test.cc"
  "lazy_cache_test.cc"
  "name_util_test.cc"
  "playlist_manager_test.cc"
  "guide_manager_test.cc"
  "scenario_manager_test.cc"
  "textures_test.cc"
  "times_test.cc"
  "util_test.cc"
  "collections_test.cc"
  "http_test.cc"
)

# Add the test executable
add_executable(${NAME} ${TEST_SOURCES})

# Link your test executable to your library and GTest
target_link_libraries(
    ${NAME}
    GTest::gmock_main
    GTest::gmock
    protobuf-matchers
    aim
)

target_compile_definitions(${NAME} PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/testdata"
)

include(../aim/warnings.cmake)
target_compile_options(${NAME} PRIVATE ${PROJECT_WARNING_FLAGS})

include(GoogleTest)
gtest_discover_tests(${NAME})
