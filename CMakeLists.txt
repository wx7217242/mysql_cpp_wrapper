
cmake_minimum_required(VERSION 2.8)

PROJECT(mysqlcppconn CXX)

SET(CMAKE_BUILD_TYPE Debug)


set(CXX_FLAGS
 -g
 # -DVALGRIND
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


set(MYSQL_CPP_CONN_SRCS
connection.cc
statement.cc
resultset.cc
)

add_library(mysqlcppconn ${MYSQL_CPP_CONN_SRCS})
target_link_libraries(mysqlcppconn mysqlclient rt)

install(TARGETS mysqlcppconn DESTINATION lib)
file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
  add_subdirectory(tests)
endif()

# 生成的可执行文件的路径
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


