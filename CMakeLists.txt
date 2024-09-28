cmake_minimum_required(VERSION 3.14)
project(RenameKeepTimestamp)

# Define source files
set(SOURCE_FILES
    src/RenameKeepTimestamp.cpp
)

# Create a static library for shared code
add_library(FileUtils STATIC src/FileUtils.cpp)

# Include directories for the shared library
target_include_directories(FileUtils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# Define resource files
set(RESOURCE_FILES
    res/Resource.rc
)

# Add the executable
add_executable(RenameKeepTimestamp WIN32 ${SOURCE_FILES} ${RESOURCE_FILES})

# Include directories
target_include_directories(RenameKeepTimestamp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(RenameKeepTimestamp
    FileUtils  # Link with the static library
    shlwapi
)

# Include directories
target_include_directories(RenameKeepTimestamp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(RenameKeepTimestamp
    shlwapi
)

# Define UNICODE macros
add_definitions(-DUNICODE -D_UNICODE)

# Set include directories for resource compiler
set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} /I ${CMAKE_CURRENT_SOURCE_DIR}/include")

# Use FetchContent to get Google Test
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git  # Ensure HTTPS is used
  GIT_TAG        v1.14.0
  GIT_SHALLOW    TRUE
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

# Define test source files
set(TEST_SOURCE_FILES
    tests/RenameKeepTimestampTest.cpp
)

# Add the test executable
add_executable(RenameKeepTimestampTest ${TEST_SOURCE_FILES})

# Include directories for tests
target_include_directories(RenameKeepTimestampTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries for tests
target_link_libraries(RenameKeepTimestampTest
    FileUtils  # Link with the static library
    GTest::gtest_main
    shlwapi
)

# Include directories for tests
target_include_directories(RenameKeepTimestampTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries for tests
target_link_libraries(RenameKeepTimestampTest
    GTest::gtest_main
    shlwapi
)

# Add tests
include(GoogleTest)
gtest_discover_tests(RenameKeepTimestampTest)
