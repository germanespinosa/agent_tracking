cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CXX_STANDARD 20)

project(agent_tracking
		VERSION 2019.1.0
		DESCRIPTION "Agent tracking service and computer vision system"
		LANGUAGES CXX)

string(APPEND CMAKE_CXX_FLAGS " -pthread ")

find_package(Dependencies)

install_dependency(https://github.com/germanespinosa/tcp-messages Tcp_messages)
install_dependency(https://github.com/germanespinosa/cellworld Cellworld)

###
### MAIN LIBRARY SETUP
###

set( agent_tracking_files
		src/tracking_service.cpp
		src/tracking_client.cpp)

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

test_library(agent_tracking catchtests/performance.cpp)

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
