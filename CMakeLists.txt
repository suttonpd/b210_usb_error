project(b2xx_usb_error)

cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE(UHD REQUIRED)

include_directories(${UHD_INCLUDE_DIRS})

add_executable(change_srate change_srate.cpp)
target_link_libraries(change_srate ${UHD_LIBRARIES})

add_executable(change_freq change_freq.cpp)
target_link_libraries(change_freq ${UHD_LIBRARIES})
