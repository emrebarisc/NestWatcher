cmake_minimum_required(VERSION 3.20)

set(PROGRAM_NAME NestWatcher)

project(${PROGRAM_NAME})

set(CMAKE_CXX_STANDARD 23)

set(PROJECT_DIR "${CMAKE_CURRENT_LIST_DIR}/")
set(SOURCE_DIR_NAME "src")
set(INCLUDE_DIR_NAME "inc")

set(SUBDIRS
	${SOURCE_DIR_NAME}
	${SOURCE_DIR_NAME}/Managers
	${SOURCE_DIR_NAME}/Network)

set(SOURCE_FILES)
foreach(SUBDIR ${SUBDIRS})
	set(COMBINED_DIR ${PROJECT_DIR}${SUBDIR})
	file(GLOB GROUP_FILES "${COMBINED_DIR}/*")
	source_group(${SUBDIR} FILES ${GROUP_FILES})
	list(APPEND SOURCE_FILES ${GROUP_FILES})
endforeach()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a -mtune=cortex-a53 -Ofast -pipe -flto -funroll-loops -fomit-frame-pointer")
endif()

add_executable(${PROGRAM_NAME} ${SOURCE_FILES})

target_include_directories(${PROGRAM_NAME} PUBLIC
	/usr/include/libcamera
	${PROJECT_DIR}/src
	${PROJECT_DIR}/${INCLUDE_DIR_NAME}
	)

target_link_libraries(${PROGRAM_NAME} PUBLIC
	camera
	camera-base
)
