cmake_minimum_required(VERSION 3.21.1)
project(2048_CPP)
set(CMAKE_CXX_STANDARD 17)

include_directories(src/include)
include_directories(Libraries/include)
include_directories(Libraries/include/FreeType)

get_filename_component(CUR_DIR "${CMAKE_CURRENT_LIST_DIR}/${MY_RELATIVE_PATH_VAR}" ABSOLUTE)

set(SRC_FILES
        ${CUR_DIR}/src/GameRendering.cpp
        ${CUR_DIR}/src/GameState.cpp
        ${CUR_DIR}/src/Graphics.cpp
        ${CUR_DIR}/src/CartesianProduct.cpp
        ${CUR_DIR}/src/TextRendering.cpp
        ${CUR_DIR}/src/Controllers/Keyboard.cpp)
if (LIB_SHARED)
    add_library(2048_CPP SHARED ${SRC_FILES})
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
else()
    add_executable(2048_CPP ${CUR_DIR}/src/main.cpp ${SRC_FILES})
endif()
if (APPLE)
    include_directories(Libraries/include/libtorch)
    include_directories(Libraries/include/libtorch/torch/csrc/api/include)

    set(GLFW ${CUR_DIR}/Libraries/lib/MacOS/glfw/libglfw.dylib)
    set(FREE_TYPE ${CUR_DIR}/Libraries/lib/MacOS/FreeType/libfreetype.dylib)
elseif (WIN32)
    set(LIB_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(GLFW ${CUR_DIR}/Libraries/lib/Windows/glfw/glfw3.lib)

    set(FREE_TYPE ${CUR_DIR}/Libraries/lib/Windows/FreeType/freetype.lib)
    list(APPEND LIBS_TO_COPY ${CUR_DIR}/Libraries/lib/Windows/FreeType/freetype.dll)

#    set(CMAKE_PREFIX_PATH "C:\\Users\\Max\\Downloads\\libtorch-win-shared-with-deps-1.13.1+cpu\\libtorch")
#    find_package(Torch REQUIRED)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
#
#    file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
#    add_custom_command(TARGET 2048_CPP
#            POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E copy_if_different
#            ${TORCH_DLLS}
#            $<TARGET_FILE_DIR:2048_CPP>
#            )
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
        2048_CPP
        ${FREE_TYPE}
        ${GLFW}
        OpenGL::GL
)
