cmake_minimum_required(VERSION 3.10)
project(LearnOpenGLcom)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories("C:/Users/gioel/Documents/Visual Studio 2019/External Libs/glad/include")
include_directories("C:/Users/gioel/Documents/Visual Studio 2019/External Libs/glfw-3.3.2.bin.WIN64/include")
include_directories("C:/Users/gioel/Documents/Visual Studio 2019/External Libs/glm-0.9.9.7")
include_directories("C:/Users/gioel/Documents/Visual Studio 2019/External Libs/assimp-5.0.1/include")

link_directories("C:/Users/gioel/Documents/Visual Studio 2019/External Libs/glfw-3.3.2.bin.WIN64/lib-vc2019")
link_directories("C:/Users/gioel/Documents/Visual Studio 2019/External Libs/assimp-5.0.1/lib/Release")

file(GLOB_RECURSE headers include/*.hpp)
file(GLOB_RECURSE sources src/*.cpp)

add_executable(LearnOpenGLcom ${headers} ${sources} src/glad.c)

target_link_libraries(LearnOpenGLcom opengl32 glfw3 assimp-vc142-mt)