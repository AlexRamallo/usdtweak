#pragma once
#include "usdtweak_api.h"

#include "Editor.h"

#include <optional>
#include <pxr/base/plug/plugin.h>
#include <pxr/base/tf/singleton.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/vt/array.h>
#include <pxr/base/vt/dictionary.h>

PXR_NAMESPACE_USING_DIRECTIVE

// Represents the entrypoint for a plugin
class USDTWEAK_API UsdTweakPluginBase : public TfWeakBase, public TfRefBase {
  public:
    // Called once when the plugin is first loaded
    virtual bool OnLoad(const VtDictionary &config) = 0;
    virtual const std::string &GetName() = 0;

    virtual inline void SetEnabled(bool set) {};
    virtual inline bool IsEnabled() { return false; };

    // Called at runtime whenever config values for this plugin are changed
    virtual void OnConfigChanged(const VtDictionary &config) {};
};

using UsdTweakPluginPtrArray = VtArray<TfRefPtr<UsdTweakPluginBase>>;

class USDTWEAK_API UsdTweakPluginRegistry {
    UsdTweakPluginRegistry(const UsdTweakPluginRegistry &) = delete;
    UsdTweakPluginRegistry &operator=(const UsdTweakPluginRegistry &) = delete;

  public:
    static UsdTweakPluginRegistry &GetInstance();

    void Add(TfRefPtr<UsdTweakPluginBase> plugin);

    const UsdTweakPluginPtrArray &GetPlugins();

    inline void SetEditor(Editor* s) {
        _editor = s;
    }
    inline Editor* GetEditor() {
        return _editor;
    }

    VtDictionary GetConfig(const std::string &plugin);
    VtValue GetConfig(const std::string &plugin, const std::string &config_key);

    template <class T> T GetConfig(const std::string &plugin, const std::string &config_key, T def) {
        VtValue value = GetConfig(plugin, config_key);
        return value.GetWithDefault<T>(def);
    }

    template <class T> std::optional<T> GetConfig(const std::string &plugin, const std::string &config_key) {
        VtValue value = GetConfig(plugin, config_key);
        if (value.IsEmpty()) {
            return std::nullopt;
        }
        return value.Get<T>();
    }

    void SetConfig(const std::string &plugin, const VtDictionary &dict);
    void SetConfig(const std::string &plugin, const std::string config_key, const VtValue &value);

    TfWeakPtr<UsdTweakPluginBase> GetPluginByName(const std::string &name);

  private:
    UsdTweakPluginRegistry();
    ~UsdTweakPluginRegistry();

    friend class TfSingleton<UsdTweakPluginRegistry>;

    UsdTweakPluginPtrArray _plugins;

    // each key is a plugin name, value is a VtDictionary
    VtDictionary _config;

    Editor* _editor = nullptr;
};