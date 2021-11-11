cmake_minimum_required(VERSION 3.5.1)
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()


set(CMAKE_CXX_STANDARD 20)

project(agent_tracking
		VERSION 2019.1.0
		DESCRIPTION "Agent cell world simulation framework"
		LANGUAGES CXX)

find_package( Cellworld REQUIRED )
find_package( OpenCV 4.0.1 REQUIRED )
find_package( Habitat_cv REQUIRED)
find_package( Easy-tcp REQUIRED)

###
### MAIN LIBRARY SETUP
###

set( agent_tracking_files
	src/camera_array.cpp
	src/frame_rate.cpp
	src/time_stamp.cpp
	src/layouts.cpp
	src/agent_tracking_service.cpp
	src/agent_tracking.cpp
	src/background.cpp include/client.h)


SET(CMAKE_CXX_FLAGS  "-DC_GNU64=400 -DOS_LINUX -no-pie -Wall -Wextra")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_library(agent_tracking ${agent_tracking_files})

target_link_libraries(agent_tracking ${OpenCV_LIBS} easy-tcp)
target_link_libraries(agent_tracking habitat_cv)
target_link_libraries(agent_tracking /usr/local/xclib/lib/pxipl_x86_64.a  /usr/local/xclib/lib/xclib_x86_64.a m -pthread )
target_link_libraries(agent_tracking cellworld)

target_include_directories(agent_tracking
		PUBLIC
		include )

target_include_directories(agent_tracking
		PUBLIC
		/usr/local/xclib/inc/ )


add_executable(agent_tracker tools/agent_tracker.cpp)
target_link_libraries(agent_tracker agent_tracking)

add_executable(sim tools/sim_episode.cpp)
target_link_libraries(sim agent_tracking)

###
### TESTS
###
find_package(CatchTests CONFIG QUIET)
