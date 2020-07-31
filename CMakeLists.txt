# To choose target architecture, variable TARGET_TYPE has to be set to
# an according value:
# 	- Linux (default)
#	- ARM
#
# For example: cmake -DCMAKE_BUILD_TYPE=Debug -DTARGET_TYPE=Linux -DPLATFORM=FC
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
PROJECT (inkdemo)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6.0)

SET (PLATFORM FC)

SET (TOOLCHAIN_PATH "../../FRSCSDK")
SET (TOOLCHAIN_PREFIX "arm-none-linux-gnueabi")
SET (TOOLCHAIN_INSTALL "sysroot/usr")

ADD_DEFINITIONS(-DPLATFORM_FC)
#SET (CMAKE_VERBOSE_MAKEFILE ON)

# building for ARM
SET (TARGET_TYPE ARM)
SET (CMAKE_BUILD_TYPE Release)

# change install prefix
SET(CMAKE_INSTALL_PREFIX "${TOOLCHAIN_PATH}/${TOOLCHAIN_PREFIX}/${TOOLCHAIN_INSTALL}" CACHE PATH "Install path prefix" FORCE)

# let's add the compiler for ARM
SET (CMAKE_C_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc)
SET (CMAKE_CXX_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++)
SET (CMAKE_LINK ${CMAKE_CURRENT_SOURCE_DIR}/${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++)
SET (CMAKE_ARR ${CMAKE_CURRENT_SOURCE_DIR}/${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-arr)
SET (CMAKE_STRIP ${CMAKE_CURRENT_SOURCE_DIR}/${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-strip)
SET (TARGET_INCLUDE "")
SET (TARGET_LIB pthread inkview freetype z)


# set needed compilation flags
MESSAGE (STATUS "Build for ARM Release")
SET (CMAKE_C_FLAGS_RELEASE "-DNDEBUG -s -fsigned-char -Wall -O2 -Wno-format-y2k -fomit-frame-pointer")
SET (CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -s -fsigned-char -Wall -O2 -Wno-format-y2k -fomit-frame-pointer")

# generate cimages/images.c
# !!! You have rebuild the project, if you add new images!!!

#SET (IMAGES_DIR ${CMAKE_SOURCE_DIR}/images)
#file(GLOB RESULT IMAGES_DIR)
#list(LENGTH RESULT IMAGES_COUNT)

#if(NOT IMAGES_COUNT EQUAL 0)
    ADD_CUSTOM_COMMAND(
                OUTPUT ${CMAKE_SOURCE_DIR}/cimages/images.c
                COMMAND ${CMAKE_SOURCE_DIR}/${TOOLCHAIN_PATH}/bin/pbres -c ${CMAKE_SOURCE_DIR}/cimages/images.c ${CMAKE_SOURCE_DIR}/images/*.bmp)
#endif()

SET (SRC_LIST
	${CMAKE_SOURCE_DIR}/src/main.cpp
        ${CMAKE_SOURCE_DIR}/src/tetris.hpp
        ${CMAKE_SOURCE_DIR}/src/tetris.cpp
        ${CMAKE_SOURCE_DIR}/src/game.hpp
        ${CMAKE_SOURCE_DIR}/src/game.cpp
        ${CMAKE_SOURCE_DIR}/src/buttons.hpp
        ${CMAKE_SOURCE_DIR}/src/buttons.cpp
        ${CMAKE_SOURCE_DIR}/src/highscore.hpp
        ${CMAKE_SOURCE_DIR}/src/highscore.cpp
        ${CMAKE_SOURCE_DIR}/src/debug.hpp
        ${CMAKE_SOURCE_DIR}/src/debug.cpp)

ADD_EXECUTABLE (inkdemo
		${SRC_LIST}
		${CMAKE_SOURCE_DIR}/cimages/images.c) 


INCLUDE_DIRECTORIES(${TARGET_INCLUDE})
TARGET_LINK_LIBRARIES (inkdemo ${TARGET_LIB})

# copying to pocketbook after build (change path if needed)
SET (targetfile "/media/gottagofaster/PB625")
add_custom_command(TARGET inkdemo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:inkdemo> ${targetfile}/applications/pbtetris.app >/dev/null || :
)

INSTALL (TARGETS inkdemo DESTINATION bin)

