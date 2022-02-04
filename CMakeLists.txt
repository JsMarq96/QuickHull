cmake_minimum_required(VERSION 3.13)

SET(CMAKE_CXX_FLAGS  "-g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(GL_GLFW_TEMPLATE)

file(GLOB CPP_SOURCES "src/*.cpp")
file(GLOB CPP_SUBFOLDER_SOURCES "src/**/*.cpp")
file(GLOB C_SOURCES "src/*.c")
file(GLOB C_SUBFOLDER_SOURCES "src/**/*.c")

set(math_lib_dir "math/")

add_executable(QUICKHULL WIN32 ${CPP_SOURCES} ${CPP_SUBFOLDER_SOURCES} ${C_SOURCES} ${C_SUBFOLDER_SOURCES})

target_include_directories(QUICKHULL PRIVATE "math/")


if( MSVC )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()


 if( MSVC )
    set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT QUICKHULL)
endif()

include_directories(
  math_lib_dir
  includes
 )

set_target_properties(QUICKHULL PROPERTIES OUTPUT_NAME "template")

add_custom_command(TARGET QUICKHULL POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:QUICKHULL>/resources)
