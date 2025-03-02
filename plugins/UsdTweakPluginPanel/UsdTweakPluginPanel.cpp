#include "UsdTweakPluginPanel.h"

#include <pxr/base/plug/registry.h>
#include <pxr/base/trace/trace.h>

#define WindowTitle "UsdTweak Plugins"

// Register plugin class with TfType system
TF_INSTANTIATE_TYPE(UsdTweakPluginPanel, TfType::CONCRETE, TF_1_PARENT(UsdTweakPluginBase));

// This is called when this plugin is loaded (after UsdTweakPluginRegistry singleton is first instantiated)
TF_REGISTRY_FUNCTION_WITH_TAG(UsdTweakPluginRegistry, UsdTweakPluginPanel) {
    UsdTweakPluginRegistry::GetInstance().Add(UsdTweakPluginPanel::New());
}

// Implement shared pointer interface (TfRefBase)
TfRefPtr<UsdTweakPluginPanel> UsdTweakPluginPanel::New() { return TfCreateRefPtr(new UsdTweakPluginPanel()); }

bool UsdTweakPluginPanel::OnLoad(const VtDictionary &config) {
    TfWeakPtr<UsdTweakPluginPanel> me(this);
    TfNotice::Register(me, &UsdTweakPluginPanel::DrawPanel);
    return true;
}

void UsdTweakPluginPanel::OnConfigChanged(const VtDictionary &config) {
    //--
}

void UsdTweakPluginPanel::DrawPanel(const EditorDrawTopLevelNotice &event) {
    TRACE_SCOPE(WindowTitle);
    ImGui::Begin(WindowTitle, &_show);

    auto &reg = UsdTweakPluginRegistry::GetInstance();
    auto plugins = reg.GetPlugins();

    for (auto &plug : plugins) {
        const auto &plugname = plug->GetName();

        bool enabled = reg.GetConfig<bool>(plugname, "enabled", true);
        if (ImGui::Checkbox(plugname.c_str(), &enabled)) {
            reg.SetConfig(plugname, "enabled", VtValue(enabled));
        }
    }

    ImGui::End();
}
