cmake_minimum_required(VERSION 2.8)


set(PROJECT_NAME "animation")
project("${PROJECT_NAME}")


# build modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/")

find_package(LibEpoxy 1.2 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLM REQUIRED glm)
pkg_search_module(ASSIMP REQUIRED assimp)


include_directories (
	${LIBEPOXY_INCLUDE_DIRS}
	${GLFW_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
)


file(GLOB SRC_FILES
    "src/*.cpp"
    "src/buffer/*.cpp"
    "src/geometry/*.cpp"
    "src/lighting/*.cpp"
    "src/load/*.cpp"
    "src/math/*.cpp"
    "src/octree/*.cpp"
    "src/scene/*.cpp"
    "src/shader/*.cpp"
    "src/spline/*.cpp"
    "src/texture/*.cpp"
    "src/view/*.cpp"
    "src/window/*.cpp"
)


# build executable
add_definitions(-std=c++17)
add_executable (GLMain ${SRC_FILES})
target_link_libraries (
	GLMain
	${LIBEPOXY_LIBRARIES}
	${GLFW_LIBRARIES}
	${GLM_LIBRARIES}
)
