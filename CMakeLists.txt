cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CXX_STANDARD 20)

project(agent_tracking
		VERSION 2019.1.0
		DESCRIPTION "Agent cell world simulation framework"
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

if (EXISTS "/usr/local/xclib/lib/pxipl_x86_64.a")
	message ("BUILDING TRACKER")
	find_package( OpenCV 4.0.1 REQUIRED )
	add_compile_definitions(XLIBALL_PRESENT "FOUND")
	install_dependency(https://github.com/germanespinosa/habitat_cv Habitat_cv)
	message("OPENCV DIRECTORIES: ${OpenCV_INCLUDE_DIRS}")
	include_directories(/usr/local/xclib/inc/
			${OpenCV_INCLUDE_DIRS})
	set ( agent_tracker_files
			src/tools/agent_tracker.cpp
			src/camera_array.cpp
			src/frame_rate.cpp
			src/layouts.cpp
			src/service.cpp
			src/agent_tracking.cpp
			src/background.cpp src/cv_service.cpp include/agent_tracking/cv_service.h)

	add_executable(agent_tracker ${agent_tracker_files})
	target_link_libraries(agent_tracker
			LINK_PUBLIC
			agent_tracking
			/usr/local/xclib/lib/pxipl_x86_64.a
			/usr/local/xclib/lib/xclib_x86_64.a m -pthread
			habitat_cv
			${OpenCV_LIBS})

else()
	message(STATUS "Building service only")
endif()

###
### TESTS
###
install_dependency(https://github.com/germanespinosa/catch CatchTests)
test_library(agent_tracking)


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
