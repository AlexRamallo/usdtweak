option(PLUGIN_COOLMENUBUTTON "Build the CoolMenuButton plugin" ON)

if(PLUGIN_COOLMENUBUTTON)
    usdtweak_plugin(CoolMenuButton SOURCES
        CoolMenuButtonPlugin.cpp
    )
endif()