
cmake_minimum_required(VERSION 2.8)

PROJECT(mysql_connector CXX)

SET(CMAKE_BUILD_TYPE Debug)

SET(TARGET mysql_test)

set(CXX_FLAGS
 -g
 # -DVALGRIND
 # -DMUDUO_STD_STRING
 -D_FILE_OFFSET_BITS=64
 -Wall
# -Wextra
 #-Werror
 #-Wconversion
 #-Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
# -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 # -std=c++0x
 -rdynamic
 )
if(CMAKE_BUILD_BITS EQUAL 32)
  list(APPEND CXX_FLAGS "-m32")
endif()
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")


string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})


# 头文件的位置
INCLUDE_DIRECTORIES(.)

# 源文件文件夹
AUX_SOURCE_DIRECTORY(. SOURCES_DIR)

# 源文件
SET(SOURCES_FILE ${SOURCES_DIR})

# 
ADD_EXECUTABLE(${TARGET} ${SOURCES_FILE})

# 生成的可执行文件的路径
# SET(EXECUTABLE_OUTPUT_PATH ${LINUX_CPP_PROJ_BINARY_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# 链接的动态库
TARGET_LINK_LIBRARIES(${TARGET} mysqlclient )

