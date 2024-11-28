# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(CustomANN)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Explicitly list all source files
set(SHARED_SOURCES
    srcs/diskindex_data.cpp
)

# Add a library with the listed source files
add_library(shared_code ${SHARED_SOURCES})

# Add the executable for the first application
add_executable(WeightCalculation srcs/edgeweightcalc.cpp)

# Add the executable for the second application
#add_executable(App2 app2.cpp)

include(FetchContent)

FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG        v2.2.1 # or use the version you prefer
)

FetchContent_MakeAvailable(cxxopts)


# Set the build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Set compiler options for Debug build
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-g)  # Add debug symbols
  add_compile_options(-O0)  # Or use -O3, -Os, etc., based on your requirements
  # Add more debug-specific flags here if needed
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  # Set optimization flags for Release build
  add_compile_options(-O2)  # Or use -O3, -Os, etc., based on your requirements
  # Add more release-specific flags here if needed
endif()



# Link the shared library with both applications
target_link_libraries(WeightCalculation PRIVATE shared_code)
target_link_libraries(WeightCalculation PRIVATE cxxopts)
