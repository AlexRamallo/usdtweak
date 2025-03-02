# The BUILD_STATIC option will result in this plugin being statically linked with usdtweak
# One benefit to this is that the plugin will be loaded automatically
usdtweak_plugin(UsdTweakPluginPanel
    BUILD_STATIC
    SOURCES
        UsdTweakPluginPanel.h
        UsdTweakPluginPanel.cpp
)