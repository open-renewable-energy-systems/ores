cmake_minimum_required(VERSION 3.10)

# Set the project name
project(MyProject)

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add an executable to the project
add_executable(${PROJECT_NAME} main.cpp)

# Add include directories
# include_directories(include)

# Link libraries
target_link_libraries(${PROJECT_NAME} ${LIBS})