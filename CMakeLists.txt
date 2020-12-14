cmake_minimum_required (VERSION 3.16)
project(poly LANGUAGES CXX)

#=============================================================================

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
	add_compile_options(/W3 /permissive-)
else()
	add_compile_options(
		-Wall -Wextra
		-Wcast-align -Wcast-qual -Wold-style-cast
		-Wconversion -Wshadow -Wstrict-overflow=3)
endif()

find_package(fmt REQUIRED)
add_subdirectory(vendor)

#=============================================================================

add_library(main STATIC src/main.cpp)
target_link_libraries(main PUBLIC Catch)
include_directories(include)
link_libraries(fmt::fmt main)

add_executable(1-inheritance src/1-inheritance.cpp)
add_executable(2-template src/2-template.cpp)
add_executable(3-functional src/3-functional.cpp)
add_executable(4-variant src/4-variant.cpp)
add_executable(5-visit src/5-visit.cpp)
add_executable(6-immutable src/6-immutable.cpp)

# virtual
# union
# std::variant
# visit
# pimpl (common interface with multiple implementations)
# overload set
# template

# duck types
# duck types with virtual dispatch
