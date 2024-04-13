cmake_minimum_required(VERSION 3.0.0)
project(asteroids VERSION 0.1.0 LANGUAGES C CXX)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(main source/main.cpp)

target_link_libraries(main PRIVATE glad::glad)
target_link_libraries(main PRIVATE glfw)
target_include_directories(main PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(main PRIVATE glm::glm)