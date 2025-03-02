cmake_minimum_required (VERSION 3.14)
project(usdtweak)

option(FORCE_STATIC_PLUGINS "Force enabled plugins to be linked statically" OFF)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

find_package(OpenGL REQUIRED)
find_package(pxr REQUIRED)
if (${PXR_VERSION} VERSION_GREATER_EQUAL 2208)
    if(UNIX AND NOT APPLE)
        find_package(X11 REQUIRED)
    endif()
    find_package(MaterialX)
endif()
set(CMAKE_CXX_STANDARD 14)
if (${PXR_VERSION} VERSION_GREATER_EQUAL 2311)
    set(CMAKE_CXX_STANDARD 17)
endif()

# if cmake can't find glfw3, clone it from github
find_package(glfw3)
if (NOT glfw3_FOUND)
    # Fix the DOWNLOAD_EXTRACT_TIMESTAMP warning on cmake 3.23+
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
        set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
    endif()
    include(FetchContent)
    FetchContent_Declare(
        glfw
        URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
    )
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_INSTALL OFF)
    set(GLFW_USE_HYBRID_HPG ON)
    FetchContent_MakeAvailable(glfw)
endif ()

set(OPENGL_gl_LIBRARY OpenGL)

# Get the stamp informations, git hash, time stamp, stuff like that
add_custom_target(stamp
  BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/src/Stamp.h ${CMAKE_CURRENT_SOURCE_DIR}/src/Stamp.cpp
  COMMAND  ${CMAKE_COMMAND}
      -DSRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
      -DDST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/stamp.cmake"
)

# Use folders in the generated xcode/vs projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 'usdteak' is a library, 'usdtweak_app' is the executable
add_library(usdtweak SHARED)
target_compile_definitions(usdtweak PRIVATE USDTWEAK_EXPORTS)

if (APPLE)
    set(MACOSX_BUNDLE_ICON_FILE icon.icns)

    # And this part tells CMake where to find and install the file itself
    set(myApp_ICON ${CMAKE_CURRENT_SOURCE_DIR}/src/resources/icon.icns)
    set_source_files_properties(${myApp_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    add_executable(usdtweak_app MACOSX_BUNDLE ${myApp_ICON})

    set_target_properties(usdtweak_app PROPERTIES
        OUTPUT_NAME "usdtweak"
        MACOSX_BUNDLE TRUE)
else()
    add_executable(usdtweak_app)
endif()
add_dependencies(usdtweak stamp)

add_subdirectory(src)

set(
    USDTWEAK_COMMON_LIBS
        glfw
        ${OPENGL_gl_LIBRARY}
        ${PXR_LIBRARIES}
        ${MATERIALX_LIBRARIES}
        $<$<CXX_COMPILER_ID:MSVC>:Shlwapi.lib>
)

target_link_libraries(usdtweak PRIVATE ${USDTWEAK_COMMON_LIBS})
target_include_directories(usdtweak PUBLIC ${OPENGL_INCLUDE_DIR} ${PXR_INCLUDE_DIRS})

set(USE_PYTHON3 ON CACHE BOOL "Compile with the Python3 target")
if (USE_PYTHON3)
    message(STATUS "Looking for Python3 target")
    find_package(Python3 COMPONENTS Development)
    target_link_libraries(usdtweak PUBLIC Python3::Python)
endif()

target_link_libraries(usdtweak_app PUBLIC resources ${USDTWEAK_COMMON_LIBS})

# Installer on windows
if(WIN32)
    include("cmake/windows_installer.cmake")
endif()

# Installer on macOS
if(APPLE)
    include("cmake/macos_installer.cmake")
endif()

# Remove warnings coming from usd and enable default multithreaded compilation on windows
target_compile_options(usdtweak PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/MP /wd4244 /wd4305 /wd4996>
    $<$<CXX_COMPILER_ID:GNU>:-Wno-deprecated>)

# NOMINMAX
target_compile_definitions(usdtweak PRIVATE NOMINMAX)

# Remove TBB deprecation warnings
add_compile_definitions(TBB_SUPPRESS_DEPRECATED_MESSAGES)

# Fix compilation error with C++17 on macos
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_STANDARD MATCHES "17")
    add_compile_definitions(_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION)
endif()


# Looking for the USD Validation API
find_file(VALIDATOR_HEADER pxr/usdValidation/usdValidation/validator.h PATHS ${PXR_INCLUDE_DIRS} NO_DEFAULT_PATH NO_SYSTEM_ENVIRONMENT_PATH)
if (VALIDATOR_HEADER)
    message(STATUS "UsdValidation API found: ${VALIDATOR_HEADER}")
    add_compile_definitions(HAVE_USDVALIDATION)
else()
    message(STATUS "UsdValidation API not found")
endif()

# Organise the sources using the same hierarchy as the filesystem in xcode and vs projects
get_target_property(USDTWEAK_SOURCES usdtweak SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "src"
    FILES ${USDTWEAK_SOURCES})



# Export the CMake targets for use by `find_package` during the build
set(USDTWEAK_CONFIG_DIR "${CMAKE_BINARY_DIR}/usdtweakConfig")
file(MAKE_DIRECTORY ${USDTWEAK_CONFIG_DIR})

# Export the library targets to a config file
export(TARGETS usdtweak resources FILE "${USDTWEAK_CONFIG_DIR}/usdtweakTargets.cmake")

add_subdirectory(plugins)