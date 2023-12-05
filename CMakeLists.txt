cmake_minimum_required(VERSION 3.16)
project(2dgame LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

include_directories(include)
add_executable(${CMAKE_PROJECT_NAME}
	src/Level.cpp
	src/World.cpp
	src/Entity.cpp
    src/main.cpp
)

# http://qrikko.blogspot.com/2016/05/cmake-and-how-to-copy-resources-during.html
set (source "${CMAKE_SOURCE_DIR}/assets")
set (destination "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
add_custom_command(
 TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
 DEPENDS ${destination}
 COMMENT "symbolic link resources folder from ${source} => ${destination}"
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE sfml-graphics)
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>
        VERBATIM)
endif()

install(TARGETS ${CMAKE_PROJECT_NAME})
