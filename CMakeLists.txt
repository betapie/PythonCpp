cmake_minimum_required(VERSION 3.16)
project(PythonCpp)

find_package(PythonLibs REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra")

include_directories(
	${PYTHON_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/include/PythonCpp
	)

file(GLOB SOURCE_FILES
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	)

add_library(PythonCpp SHARED ${SOURCE_FILES})

target_link_libraries(PythonCpp ${PYTHON_LIBRARIES})
