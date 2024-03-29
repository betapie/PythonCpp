cmake_minimum_required(VERSION 3.16)
project(PythonCpp)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

option(BUILD_SAMPLES "Enable or disable building the samples" OFF)
option(BUILD_TESTS "Enable or disable building tests" OFF)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra")
endif()

include_directories(
	${Python3_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/include/PythonCpp
	)

file(GLOB SOURCE_FILES
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	)

add_library(PythonCpp SHARED ${SOURCE_FILES})

target_compile_definitions(PythonCpp PRIVATE PYCPP_EXPORTS)

target_link_libraries(PythonCpp ${Python3_LIBRARIES})

if(BUILD_SAMPLES)
	add_executable(pyplot ${PROJECT_SOURCE_DIR}/samples/pyplot.cpp)

	target_link_libraries(pyplot PUBLIC PythonCpp)
	target_include_directories(pyplot PUBLIC ${PROJECT_SOURCE_DIR}/include/PythonCpp)
endif()

if(BUILD_TESTS)
	include(FetchContent)
	FetchContent_Declare(
		googletest
		URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
	)

	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)

	enable_testing()

	add_executable(
		PythonCppTests
		"tests/PythonTypeTraitsTests.cpp"
		)

	target_link_libraries(PythonCppTests
		gtest_main
		PythonCpp
		)

	target_include_directories(PythonCppTests PUBLIC ${PROJECT_SOURCE_DIR}/include/PythonCpp)

	include(GoogleTest)
	gtest_discover_tests(PythonCppTests)
endif()