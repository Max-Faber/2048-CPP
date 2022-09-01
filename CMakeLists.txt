cmake_minimum_required(VERSION 3.21.1)
project(2048_C__)
set(CMAKE_CXX_STANDARD 17)

include_directories(src/include)
include_directories(Libraries/include)

get_filename_component(CUR_DIR "${CMAKE_CURRENT_LIST_DIR}/${MY_RELATIVE_PATH_VAR}" ABSOLUTE)

add_executable(
        2048_C__
        src/main.cpp
        src/GameDrawing.cpp
        src/GameLogic.cpp
        src/Graphics.cpp
        src/CartesianProduct.cpp
)

if (APPLE)
    set(GLUT "-framework GLUT -framework OpenGL")
elseif (WIN32)
    set(LIB_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(GLUT_DIR ${CUR_DIR}/Libraries/lib)
    set(GLUT ${GLUT_DIR}/glut32.lib)
    list(APPEND LIBS_TO_COPY ${GLUT_DIR}/glut32.dll)
endif ()

foreach (path ${LIBS_TO_COPY})
    message("path: ${path}")
    get_filename_component(filename ${path} NAME)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${path}
            ${LIB_DIR}/${filename}
    )
endforeach ()

target_link_libraries(
        2048_C__
        ${GLUT}
)
