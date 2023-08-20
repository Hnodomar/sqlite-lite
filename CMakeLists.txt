cmake_minimum_required(VERSION 3.20)

PROJECT(sqlite-lite)

file(GLOB sqlite-lite_src CONFIGURE_DEPENDS
     "src/cli/*.cpp")

add_executable(sqll_cli ${sqlite-lite_src})

target_include_directories(sqll_cli PUBLIC ${CMAKE_SOURCE_DIR}/include)
  
message(${CMAKE_SOURCE_DIR}/include)

