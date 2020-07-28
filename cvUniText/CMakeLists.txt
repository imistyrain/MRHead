cmake_minimum_required(VERSION 2.8)
project(cvUniText)

add_library(cvunitext cvUniText.cpp)
add_executable(main test.cpp)
target_link_libraries(main cvunitext)
target_link_libraries(cvunitext freetype)

if(ANDROID)
    include_directories(${OPENCV_DIR}/include)
    target_link_libraries(main opencv android)
else()
    find_package(OpenCV REQUIRED)
    find_package(freetype)
    include_directories(${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(main ${OpenCV_LIBS})
endif()

file(COPY microhei.ttc DESTINATION ${CMAKE_BINARY_DIR})