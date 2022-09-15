cmake_minimum_required(VERSION 3.21.1)
project(2048_C__)
set(CMAKE_CXX_STANDARD 17)

include_directories(src/include)
include_directories(Libraries/include)
include_directories(Libraries/include/FreeType)

get_filename_component(CUR_DIR "${CMAKE_CURRENT_LIST_DIR}/${MY_RELATIVE_PATH_VAR}" ABSOLUTE)

add_executable(
        2048_C__
        src/main.cpp
        src/GameRendering.cpp
        src/GameLogic.cpp
        src/Graphics.cpp
        src/CartesianProduct.cpp
        src/InputControl.cpp
        src/TextRendering.cpp
#        Libraries/lib/glad/glad.c
)

if (APPLE)
    set(GLUT "-framework GLUT -framework OpenGL")
#    link_libraries(${CUR_DIR}/Libraries/lib/MacOS/FreeType/libfreetype.6.dylib)
    set(FREE_TYPE ${CUR_DIR}/Libraries/lib/MacOS/FreeType/libfreetype.dylib)
#    link_libraries(${CUR_DIR}/Libraries/lib/MacOS/FreeType/libfreetype.dylib)
elseif (WIN32)
    set(LIB_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(GLUT_DIR ${CUR_DIR}/Libraries/lib/Windows/GLUT)
    set(GLUT ${GLUT_DIR}/glut32.lib)
    list(APPEND LIBS_TO_COPY ${GLUT_DIR}/glut32.dll)

    set(FREE_TYPE ${CUR_DIR}/Libraries/lib/Windows/FreeType/freetype.lib)
    list(APPEND LIBS_TO_COPY ${CUR_DIR}/Libraries/lib/Windows/FreeType/freetype.pdb)
endif ()

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CUR_DIR}/Dependencies/fonts/arial.ttf
        ${CMAKE_BINARY_DIR}/fonts/arial.ttf
)

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
        ${FREE_TYPE}
)
