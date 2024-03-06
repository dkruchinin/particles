cmake_minimum_required(VERSION 3.16)
project(particles)

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"src/particle.cpp"
	"src/pconfig.cpp"
	"src/simulation.cpp"
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
target_compile_features(${PROJECT_NAME}
	PRIVATE cxx_std_11
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

file(COPY "${CMAKE_SOURCE_DIR}/configs" DESTINATION ${CMAKE_BINARY_DIR})
