
target_sources(usdtweak PRIVATE
 ${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_glfw.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_opengl3.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/imgui_stdlib.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/imgui_tables.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp
)

target_include_directories(usdtweak PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    target_compile_definitions(usdtweak PRIVATE "IMGUI_API=__declspec(dllexport)")
    target_compile_definitions(usdtweak PRIVATE "IMGUI_IMPL_API=__declspec(dllexport)")
else()
    target_compile_definitions(usdtweak PRIVATE "IMGUI_API=__attribute__((visibility(\"default\")))")
    target_compile_definitions(usdtweak PRIVATE "IMGUI_IMPL_API=__attribute__((visibility(\"default\")))")
endif()