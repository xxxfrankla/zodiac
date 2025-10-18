cmake_minimum_required(VERSION 3.18)

project(astrocore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED REACT_NATIVE_DIR)
  get_filename_component(REACT_NATIVE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../node_modules/react-native"
    REALPATH)
endif()

set(JSI_DIR "${REACT_NATIVE_DIR}/ReactCommon/jsi")
set(RCTBENCHMARK_DIR "${REACT_NATIVE_DIR}/ReactCommon")

file(GLOB ASTROCORE_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/../cpp/src/*.cpp"
)

add_library(astrocore SHARED ${ASTROCORE_SOURCES})

target_include_directories(astrocore
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../cpp/include"
    "${JSI_DIR}"
    "${RCTBENCHMARK_DIR}"
)

find_library(log-lib log)
find_library(fbjni-lib fbjni)
find_library(jsi-lib jsi)

target_link_libraries(astrocore
  ${log-lib}
  ${fbjni-lib}
  ${jsi-lib}
)
