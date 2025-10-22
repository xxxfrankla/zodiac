cmake_minimum_required(VERSION 3.18)

project(astrocore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(astrocore STATIC
  src/engine.cpp
  src/time.cpp
  src/transform.cpp
  src/vector.cpp
)

target_include_directories(astrocore PUBLIC include)

target_compile_options(astrocore PRIVATE
  -Wall -Wextra -Wpedantic
)

enable_testing()

function(add_astro_test name)
  add_executable(${name} tests/${name}.cpp)
  target_link_libraries(${name} PRIVATE astrocore)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

add_astro_test(test_time)
add_astro_test(test_transform)
add_astro_test(test_engine)
