cmake_minimum_required(VERSION 3.8.0)

project(cloudabi-demo-webserver)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build test programs, using Google Test" ON)

find_package(Threads REQUIRED)

set(APROTOC_NAME "aprotoc" CACHE STRING "Name of the aprotoc command")
find_program(APROTOC_COMMAND ${APROTOC_NAME})
mark_as_advanced(APROTOC_COMMAND)
if(NOT APROTOC_COMMAND)
	message(FATAL_ERROR "Could not find aprotoc, set APROTOC_NAME")
endif()

function(add_aprotoc base)
	get_filename_component(base_dir ${base} DIRECTORY)
	add_custom_command(
		OUTPUT ${base}.ad.h
		COMMAND mkdir -p ${CMAKE_BINARY_DIR}/${base_dir} && ${APROTOC_COMMAND} <${CMAKE_SOURCE_DIR}/${base}.proto >${CMAKE_BINARY_DIR}/${base}.ad.h
		DEPENDS ${CMAKE_SOURCE_DIR}/${base}.proto
	)
endfunction()

include_directories(${CMAKE_BINARY_DIR})

add_aprotoc(configuration)

add_executable(
	cloudabi-demo-webserver
	configuration.ad.h
	fixed_response_server.cc
	program_main.cc
)

target_link_libraries(
	cloudabi-demo-webserver
	argdata
	arpc
)

install(
	TARGETS cloudabi-demo-webserver
	DESTINATION ${CMAKE_INSTALL_BINDIR}
)
