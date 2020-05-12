# CMakeList.txt: проект CMake для BloodCounterCC; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

project(Dungeon_runner)

find_package(OpenGL REQUIRED)

add_subdirectory(glfw)
add_subdirectory(freetype)
add_subdirectory(glm)

include_directories(glfw/include)
include_directories(freetype/include)
include_directories(include)
include_directories(glm)


set(LIBS glfw freetype glm ${GLFW_LIBRARIES} ${OPENGL_LIBRARY})

add_executable(settle_test tests/settle_test.cpp src/glad.c)
target_link_libraries(settle_test ${LIBS})

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/res/ )
file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
