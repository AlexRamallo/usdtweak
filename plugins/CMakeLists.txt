# copy 'root' plugInfo which just wildcard imports all plugin plugInfo.json files
configure_file(plugInfo.json ${CMAKE_BINARY_DIR}/plugInfo.json COPYONLY)

# Helper function for building plugins
function(usdtweak_plugin PLUG_NAME)
	cmake_parse_arguments(PARSE_ARGV 1 ARGS "BUILD_STATIC" "" "SOURCES")
	if(ARGS_BUILD_STATIC OR FORCE_STATIC_PLUGINS)
		# just add sources to usdtweak target to skip making static library for plugin
		target_sources(usdtweak PRIVATE ${ARGS_SOURCES})
	else()
		# Build plugin as shared library
		add_library(${PLUG_NAME} SHARED ${ARGS_SOURCES})
		target_link_libraries(${PLUG_NAME} PUBLIC usdtweak)
		# create plugInfo.json with correct "LibraryPath" key
		set(PLUG_LIB_PATH "${CMAKE_SHARED_LIBRARY_PREFIX}${PLUG_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}")
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/plugInfo.json ${CMAKE_CURRENT_BINARY_DIR}/plugInfo.json @ONLY)
	endif()
endfunction()

# Plugins
add_subdirectory(CoolMenuButton)
add_subdirectory(UsdTweakPluginPanel)
add_subdirectory(PrimFinder)

