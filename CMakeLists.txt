cmake_minimum_required(VERSION 3.5.1)
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()


set(CMAKE_CXX_STANDARD 17)

project(agent_tracking
		VERSION 2019.1.0
		DESCRIPTION "Agent cell world simulation framework"
		LANGUAGES CXX)

find_package( Cellworld REQUIRED )
find_package( OpenCV 4.0.1 REQUIRED )
find_package( Habitat_cv REQUIRED)

###
### MAIN LIBRARY SETUP
###

set( agent_tracking_files
	 src/camera_array.cpp
		src/input_handler.cpp include/frame_rate.h src/frame_rate.cpp include/mask.h src/mask.cpp include/time_stamp.h src/time_stamp.cpp include/classifier.h tools/robot_controller/src/robot_controller.cpp)


SET(CMAKE_CXX_FLAGS  "-DC_GNU64=400 -DOS_LINUX -no-pie -Wall -Wextra")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_library(agent_tracking ${agent_tracking_files})

target_link_libraries(agent_tracking ${OpenCV_LIBS})
target_link_libraries(agent_tracking habitat_cv)
target_link_libraries(agent_tracking /usr/local/xclib/lib/pxipl_x86_64.a  /usr/local/xclib/lib/xclib_x86_64.a m -pthread )
target_link_libraries(agent_tracking cellworld)

target_include_directories(agent_tracking
		PUBLIC
		include )

target_include_directories(agent_tracking
		PUBLIC
		/usr/local/xclib/inc/ )


add_executable(robot_controller tools/robot_controller/src/robot_controller.cpp tools/robot_controller/include/controller.h tools/robot_controller/include/ptcp.h tools/robot_controller/src/controller.cpp tools/robot_controller/src/ptcp.cpp)
target_link_libraries(robot_controller agent_tracking)


add_executable(agent_tracker tools/agent_tracker.cpp)
target_link_libraries(agent_tracker agent_tracking)

add_executable(capture tools/capture.cpp)
target_link_libraries(capture agent_tracking)

add_executable(speed_test tools/speed_test.cpp)
target_link_libraries(speed_test agent_tracking)


#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

###
### TESTS
###
find_package(CatchTests CONFIG QUIET)
