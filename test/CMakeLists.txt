find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        URL_HASH SHA256=40d4ec942217dcc84a9ebe2a68584ada7d4a33a8ee958755763278ea1c5e18ff
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(guided_openmp_tests MatrixProductTests.cpp)
target_link_libraries(
    guided_openmp_tests
    PRIVATE GuidedRepositoryOpenMP::guided_openmp GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(guided_openmp_tests)
