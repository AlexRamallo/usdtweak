#include "Editor.h"
#include "UsdTweakPluginRegistry.h"

#include <iostream>

#include <pxr/base/tf/instantiateType.h>
#include <pxr/base/tf/notice.h>

PXR_NAMESPACE_USING_DIRECTIVE

class CoolMenuButtonPlugin : public UsdTweakPluginBase {
  public:
    static TfRefPtr<CoolMenuButtonPlugin> New() { return TfCreateRefPtr(new CoolMenuButtonPlugin()); }

    const std::string &GetName() override { return _name; }

    bool OnLoad(const VtDictionary &config) override {
        TfWeakPtr<CoolMenuButtonPlugin> me(this);
        // store registration key so we can disconnect it later
        _key = TfNotice::Register(me, &CoolMenuButtonPlugin::DrawButton);
        return true;
    }

    void OnConfigChanged(const VtDictionary &config) override {
        TfWeakPtr<CoolMenuButtonPlugin> me(this);

        // TODO: this is ugly

        const VtValue *v_enabled = config.GetValueAtPath("enabled");
        bool enabled = (v_enabled && !v_enabled->IsEmpty()) ? v_enabled->Get<bool>() : true;

        if (enabled && !_key.IsValid()) {
            _key = TfNotice::Register(me, &CoolMenuButtonPlugin::DrawButton);
        } else if (!enabled && _key.IsValid()) {
            TfNotice::Revoke(_key);
        }
    }

    // EditorDrawMainMenuBarNotice is fired by `Editor::DrawMainMenuBar` every frame
    void DrawButton(const EditorDrawMainMenuBarNotice &event) {
        if (ImGui::Button("This is a plugin!")) {
            std::cout << "Clicked the plugin button " << ++_clickCount << " times!\n";
        }
    }

  private:
    CoolMenuButtonPlugin() = default;
    ~CoolMenuButtonPlugin() = default;

    std::string _name = "CoolMenuButtonPlugin";
    TfNotice::Key _key;
    int _clickCount = 0;
};

TF_INSTANTIATE_TYPE(CoolMenuButtonPlugin, TfType::CONCRETE, TF_1_PARENT(UsdTweakPluginBase));
TF_REGISTRY_FUNCTION_WITH_TAG(UsdTweakPluginRegistry, CoolMenuButtonPlugin) {
    UsdTweakPluginRegistry::GetInstance().Add(CoolMenuButtonPlugin::New());
}