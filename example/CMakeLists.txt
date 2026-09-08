add_executable(regular_new regular_new.cpp)
target_link_libraries(regular_new PRIVATE GuidedRepositoryOpenMP::guided_openmp)

add_executable(overloaded_new overloaded_new.cpp)
target_link_libraries(
    overloaded_new
    PRIVATE GuidedRepositoryOpenMP::guided_openmp CustomMemoryAllocator::global_new
)
