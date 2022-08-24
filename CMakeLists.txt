cmake_minimum_required(VERSION 3.22)
project(VertexAnimation)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions("HAVE_BOOLEAN=1")
list(APPEND CMAKE_PREFIX_PATH "./thirdparty/glew-2.1.0")
list(APPEND CMAKE_PREFIX_PATH "./thirdparty/freeglut")
list(APPEND CMAKE_PREFIX_PATH "./thirdparty/libjpeg")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(JPEG REQUIRED)

add_compile_options(-fpermissive)

include_directories(${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR} ${JPEG_INCLUDE_DIR})

add_executable(VertexAnimation
        main.cpp
        console.h
        console.cpp
        math/vector.cpp
        math/vector.h
        math/mat4.h
        math/mat4.cpp
        timer.h
        timer.cpp
        textures.h
        textures.cpp
        shaders.h
        shaders.cpp
        xmlevelshaders.cpp
        xmlevelshaders.h
        xmvertexanimation.h
        xmvertexanimation.cpp
        directx/ddraw.h main.h)

target_link_libraries(VertexAnimation JPEG::JPEG  ${OPENGL_LIBRARIES} GLUT::GLUT GLEW::GLEW)