set(ProjName test)
project(${ProjName})
cmake_minimum_required(VERSION 2.8.11)

AUX_SOURCE_DIRECTORY(. SRC_LIST)
add_executable(${ProjName} ${SRC_LIST})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(WIN32)
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../tickmeter.hpp" DESTINATION "include")
else()
	find_package(OpenCV REQUIRED)
	include_directories(${OPENCV_DIRS})
	target_link_libraries (${ProjName} ${OpenCV_LIBS})
endif()

# copy png for test
file(GLOB pngs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.png")
file(COPY ${pngs}  DESTINATION ${CMAKE_BINARY_DIR})

#copy to system include
file(GLOB headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../*.h")
install(FILES ${headers} DESTINATION "include")