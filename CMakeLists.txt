cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CXX_STANDARD 20)

project(agent_tracking
		VERSION 2019.1.0
		DESCRIPTION "Agent tracking service and computer vision system"
		LANGUAGES CXX)

find_package(Dependencies)

install_dependency(https://github.com/germanespinosa/tcp-messages Tcp_messages)
install_dependency(https://github.com/germanespinosa/cellworld Cellworld)

###
### MAIN LIBRARY SETUP
###

set( agent_tracking_files
	src/service.cpp
	src/client.cpp )

string(APPEND CMAKE_CXX_FLAGS " -DC_GNU64=400 -DOS_LINUX -no-pie -Wall -Wextra -pthread ")
string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O3 ")

add_library(agent_tracking ${agent_tracking_files})

target_link_libraries(agent_tracking
		LINK_PUBLIC
		cellworld
		tcp_messages )

dependency_include(include)

###
### TESTS
###
install_dependency(https://github.com/germanespinosa/catch CatchTests)
test_library(agent_tracking catchtests/tracking_service.cpp)

if (EXISTS "/usr/local/xclib/lib/pxipl_x86_64.a")
	message ("BUILDING TRACKER")
	install_dependency(https://github.com/germanespinosa/habitat_cv Habitat_cv)
	include_directories(/usr/local/xclib/inc/)
	set ( agent_tracker_files
			src/tools/agent_tracker.cpp
			src/camera_array.cpp
			src/frame_rate.cpp
			src/layouts.cpp
			src/service.cpp
			src/agent_tracking.cpp
			src/background.cpp
			src/cv_service.cpp)

	add_executable(agent_tracker ${agent_tracker_files})
	target_link_libraries(agent_tracker
			LINK_PUBLIC
			agent_tracking
			habitat_cv
			/usr/local/xclib/lib/pxipl_x86_64.a
			/usr/local/xclib/lib/xclib_x86_64.a m -pthread)

else()
	message(STATUS "Building service only")
endif()

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

install(CODE "FILE(APPEND ${CMAKE_INSTALL_FULL_DATADIR}/cmake/Agent_tracking/Agent_trackingConfig.cmake \"set(AGENT_TRACKING_LIBRARIES ${CELLWORLD_LIBRARIES} agent_tracking)\n\") ")
