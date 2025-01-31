cmake_minimum_required (VERSION 3.16)

project (JHU_CS_605.668)

if(WIN32)
    set(BUILD_MS_WINDOWS 1)
    add_definitions(-DBUILD_WINDOWS)
    set(WINDOW_ARCH "x64" CACHE STRING "Windows Architecture")
    set_property(CACHE WINDOW_ARCH PROPERTY STRINGS "x64;x86;arm64")
elseif(APPLE)
    set(BUILD_MACOS 1)
    add_definitions(-DBUILD_MACOS)
elseif(UNIX AND NOT APPLE)
    set(BUILD_LINUX 1)
    add_definitions(-DBUILD_LINUX)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(BUILD_MS_WINDOWS)
    set(SRC_DIR_WIN \"$(ProjectDir)..\")
    set(RESOURCE_DIR_WIN \"$(ProjectDir)..\\resources\" )
    add_definitions(-DSRC_DIR=${SRC_DIR_WIN})
    add_definitions(-DRESOURCE_DIR=${RESOURCE_DIR_WIN})
    message("SRC_DIR_WIN: " ${SRC_DIR_WIN})
    message("RESOURCE_DIR_WIN: " ${RESOURCE_DIR_WIN})
else()
    add_definitions(-DSRC_DIR=${CMAKE_SOURCE_DIR})
    add_definitions(-DRESOURCE_DIR=${CMAKE_SOURCE_DIR}/resources)
endif()

if(BUILD_MS_WINDOWS)
    unset(SDL_LIB CACHE)
    set(SDL_PATH ${CMAKE_SOURCE_DIR}/lib/win/SDL3/${WINDOW_ARCH})
    find_library(SDL_LIB NAMES SDL3 PATHS ${SDL_PATH})
elseif(BUILD_MACOS)
    find_library(SDL_LIB NAMES SDL3 PATHS ${CMAKE_SOURCE_DIR}/lib/macos)
elseif(BUILD_LINUX)
    find_library(SDL_LIB NAMES SDL3 PATHS ${CMAKE_SOURCE_DIR}/lib/linux/SDL3)
    find_library(PTHREAD_LIBRARY NAMES pthread)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

set(SUB_LIB_LIST "")

add_subdirectory(src/examples)
add_subdirectory(src/graph)
add_subdirectory(src/image)
add_subdirectory(src/platform)
add_subdirectory(src/system)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    ${SUB_LIB_LIST}
    ${SDL_LIB}
    ${CMAKE_DL_LIBS} 
    ${PTHREAD_LIBRARY})
        
if(BUILD_MS_WINDOWS)
    file(GLOB DLLS_TO_MOVE 
        ${SDL_PATH}/SDL3.dll
    )

    foreach( file_i ${DLLS_TO_MOVE})
        string(REPLACE "/" "\\" FILE_I_PATH ${file_i} )
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND Copy /Y \"${FILE_I_PATH}\" \"$(TargetDir)\"
            COMMENT "Move dll to output directory" 
        )
    endforeach( file_i )
endif()
