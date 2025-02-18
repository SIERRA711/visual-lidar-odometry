cmake_minimum_required(VERSION 3.10)
project(violasint)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Manually specify PDAL and OpenCV paths
set(PDAL_DIR "C:/msys64/mingw64/lib/cmake/PDAL")
set(OpenCV_DIR "C:/msys64/mingw64/lib/cmake/opencv4")

# Find packages
find_package(PDAL REQUIRED)
find_package(OpenCV REQUIRED)

# Optionally, check what OpenCV found:
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")

# Define the executable
add_executable(violasint violasint.cpp)

# Add include directories and link libraries for this target
target_include_directories(violasint PRIVATE 
    ${PDAL_INCLUDE_DIRS} 
    ${OpenCV_INCLUDE_DIRS} 
    "C:/msys64/mingw64/include/opencv4"  # Ensure opencv4 is in the path
)

target_link_libraries(violasint 
    ${PDAL_LIBRARIES} 
    ${OpenCV_LIBS}
)