cmake_minimum_required(VERSION 3.21.1)
project(2048_C__)
set(CMAKE_CXX_STANDARD 17)

include_directories(src/include)
include_directories(Libraries/include)
include_directories(Libraries/include/FreeType)
include_directories(Libraries/include/libtorch)
include_directories(Libraries/include/libtorch/torch/csrc/api/include)

get_filename_component(CUR_DIR "${CMAKE_CURRENT_LIST_DIR}/${MY_RELATIVE_PATH_VAR}" ABSOLUTE)

add_executable(
        2048_C__
        src/main.cpp
        src/GameRendering.cpp
        src/GameState.cpp
        src/Graphics.cpp
        src/CartesianProduct.cpp
        src/TextRendering.cpp
        src/Controllers/Keyboard.cpp
        src/Controllers/AI/DQN.cpp)

if (APPLE)
    set(GLFW ${CUR_DIR}/Libraries/lib/MacOS/glfw/libglfw.dylib)
    set(FREE_TYPE ${CUR_DIR}/Libraries/lib/MacOS/FreeType/libfreetype.dylib)
    set(TORCH ${CUR_DIR}/Libraries/lib/MacOS/libtorch/libtorch_cpu.dylib)
    set(C10 ${CUR_DIR}/Libraries/lib/MacOS/libtorch/libc10.dylib)
elseif (WIN32)
    set(LIB_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(GLFW ${CUR_DIR}/Libraries/lib/Windows/glfw/glfw3.lib)

    set(FREE_TYPE ${CUR_DIR}/Libraries/lib/Windows/FreeType/freetype.lib)
    list(APPEND LIBS_TO_COPY ${CUR_DIR}/Libraries/lib/Windows/FreeType/freetype.pdb)
    list(APPEND LIBS_TO_COPY ${CUR_DIR}/Libraries/lib/Windows/FreeType/freetype.dll)
endif ()

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CUR_DIR}/Dependencies/fonts/arial.ttf
        ${CMAKE_BINARY_DIR}/fonts/arial.ttf
)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CUR_DIR}/Dependencies/fonts/SmallMemory.ttf
        ${CMAKE_BINARY_DIR}/fonts/SmallMemory.ttf
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

find_package(OpenGL REQUIRED)

target_link_libraries(
        2048_C__
        ${FREE_TYPE}
        ${GLFW}
        ${TORCH}
        ${C10}
        OpenGL::GL
)
