cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CXX_STANDARD 20)

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
	src/frame_rate.cpp
	src/time_stamp.cpp
	src/layouts.cpp
	src/service.cpp
	src/agent_tracking.cpp
	src/background.cpp
	src/client.cpp)


string(APPEND CMAKE_CXX_FLAGS " -DC_GNU64=400 -DOS_LINUX -no-pie -Wall -Wextra ")
string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O3 ")

add_library(agent_tracking ${agent_tracking_files})

target_link_libraries(agent_tracking ${OpenCV_LIBS} ${CELLWORLD_LIBRARIES} habitat_cv)
target_link_libraries(agent_tracking /usr/local/xclib/lib/pxipl_x86_64.a  /usr/local/xclib/lib/xclib_x86_64.a )

target_include_directories(cellworld
		SYSTEM INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>)

target_include_directories(agent_tracking
		PRIVATE
		include )

target_include_directories(agent_tracking
		PUBLIC
		/usr/local/xclib/inc/ )

add_executable(agent_tracker src/tools/agent_tracker.cpp)
target_link_libraries(agent_tracker agent_tracking)

add_executable(sim src/tools/sim_episode.cpp)
target_link_libraries(sim agent_tracking)

###
### TESTS
###
find_package(CatchTests CONFIG QUIET)


###
### LIBRARY INSTALLATION
###

include(GNUInstallDirs)

install(TARGETS agent_tracking EXPORT Agent_trackingConfig
		ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT Agent_trackingConfig
		DESTINATION ${CMAKE_INSTALL_DATADIR}/cmake/Agent_tracking
		EXPORT_LINK_INTERFACE_LIBRARIES)

export(TARGETS agent_tracking FILE Agent_trackingConfig.cmake)
install(CODE "FILE(APPEND ${CMAKE_INSTALL_FULL_DATADIR}/cmake/Agent_tracking/Agent_trackingConfig.cmake \"find_package(Cellworld REQUIRED)\n\") ")
install(CODE "FILE(APPEND ${CMAKE_INSTALL_FULL_DATADIR}/cmake/Agent_tracking/Agent_trackingConfig.cmake \"find_package(OpenCV REQUIRED)\n\") ")
install(CODE "FILE(APPEND ${CMAKE_INSTALL_FULL_DATADIR}/cmake/Agent_tracking/Agent_trackingConfig.cmake \"find_package(Habitat_cv REQUIRED)\n\") ")

install(CODE "FILE(APPEND ${CMAKE_INSTALL_FULL_DATADIR}/cmake/Agent_tracking/Agent_trackingConfig.cmake \"set(AGENT_TRACKING_LIBRARIES ${OpenCV_LIBS} ${CELLWORLD_LIBRARIES} habitat_cv agent_tracking)\n\") ")
