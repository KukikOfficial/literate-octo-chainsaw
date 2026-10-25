cmake_minimum_required(VERSION 3.20)
project(MiniEngine CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# 2. GLM
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)

# 3. Dear ImGui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91.5
)

# 4. ImGuizmo
FetchContent_Declare(
        imguizmo
        GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
        GIT_TAG master
)

# Отключаем сборку примеров ImGuizmo, чтобы не тянуть лишнее
set(IMGUIZMO_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)

# Скачиваем все зависимости штатным способом
FetchContent_MakeAvailable(glfw glm imgui imguizmo)

# Собираем статическую библиотеку для ImGui бэкендов (так как у ImGui нет своего CMakeLists.txt)
add_library(imgui_backends STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_backends PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui_backends PUBLIC glfw OpenGL::GL)

# Если цель imguizmo создана своим CMakeLists, настраиваем инклюды ImGui
if(TARGET imguizmo)
    target_include_directories(imguizmo PUBLIC ${imgui_SOURCE_DIR})
    target_link_libraries(imguizmo PUBLIC imgui_backends)
else()
    # Фоллбэк на случай старой версии репозитория без CMakeLists.txt
    add_library(imguizmo STATIC ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp)
    target_include_directories(imguizmo PUBLIC ${imguizmo_SOURCE_DIR} ${imgui_SOURCE_DIR})
    target_link_libraries(imguizmo PUBLIC imgui_backends)
endif()

find_package(OpenGL REQUIRED)

set(SOURCES
        src/glad.c
        src/Camera.cpp
        src/Shader.cpp
        src/MeshRenderer.cpp
        src/main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        imguizmo
        imgui_backends
        glfw
        glm::glm
        OpenGL::GL
)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)