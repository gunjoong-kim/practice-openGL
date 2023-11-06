cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME opengl_example)
set(CMAKE_CXX_STANDARD 17)

set(WINDOW_NAME "OpenGL Example")
set(WINDOW_WIDTH 960)
set(WINDOW_HEIGHT 540)

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME}
	src/main.cpp
	src/Common.cpp inc/Common.hpp
	src/Shader.cpp inc/Shader.hpp
	src/Program.cpp inc/Program.hpp
	src/Context.cpp inc/Context.hpp
	src/Buffer.cpp inc/Buffer.hpp
	src/VertexLayout.cpp inc/VertexLayout.hpp
	src/Image.cpp inc/Image.hpp
	src/Texture.cpp inc/Texture.hpp
	)

include(Dependency.cmake)

# 우리 프로젝트에 include / lib 관련 옵션 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC inc)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
    )

# Dependency들이 먼저 build 될 수 있게 관계 설정
add_dependencies(${PROJECT_NAME} ${DEP_LIST})