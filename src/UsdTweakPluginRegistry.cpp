#include "UsdTweakPluginRegistry.h"
#include <iostream>

#include <pxr/base/tf/instantiateSingleton.h>
#include <pxr/base/tf/instantiateType.h>

TF_INSTANTIATE_TYPE(UsdTweakPluginBase, TfType::ABSTRACT, TF_NO_PARENT());

TF_INSTANTIATE_SINGLETON(UsdTweakPluginRegistry);

UsdTweakPluginRegistry::UsdTweakPluginRegistry() : _plugins{}, _config{} {
    std::cout << "UsdTweakPluginRegistry::ctor\n";

    TfSingleton<UsdTweakPluginRegistry>::SetInstanceConstructed(*this);
    TfRegistryManager::GetInstance().SubscribeTo<UsdTweakPluginRegistry>();
}

UsdTweakPluginRegistry::~UsdTweakPluginRegistry() {
    std::cout << "UsdTweakPluginRegistry::dtor\n";
    TfRegistryManager::GetInstance().UnsubscribeFrom<UsdTweakPluginRegistry>();
}

UsdTweakPluginRegistry &UsdTweakPluginRegistry::GetInstance() { return TfSingleton<UsdTweakPluginRegistry>::GetInstance(); }

void UsdTweakPluginRegistry::Add(TfRefPtr<UsdTweakPluginBase> plugin) {
    std::cout << "UsdTweakPluginRegistry::Add(" << plugin->GetName() << ")\n";
    _plugins.push_back(plugin);

    VtDictionary config; // TODO: load this from somewhere
    if (plugin->OnLoad(config)) {
        plugin->SetEnabled(true);
    } else {
        std::cerr << "Failed to load usdtweak plugin: " << plugin->GetName() << "\n";
    }
}

const UsdTweakPluginPtrArray &UsdTweakPluginRegistry::GetPlugins() { return _plugins; }

TfWeakPtr<UsdTweakPluginBase> UsdTweakPluginRegistry::GetPluginByName(const std::string &name) {
    for (auto &plug : _plugins) {
        if (!plug->GetName().compare(name)) {
            return plug;
        }
    }
    return nullptr;
}

void UsdTweakPluginRegistry::SetConfig(const std::string &plugin, const VtDictionary &dict) {
    if (auto plug = GetPluginByName(plugin)) {
        _config[plugin] = dict;
        plug->OnConfigChanged(dict);
    }
}

void UsdTweakPluginRegistry::SetConfig(const std::string &plugin, const std::string config_key, const VtValue &value) {
    if (auto plug = GetPluginByName(plugin)) {
        VtDictionary dict = _config[plugin].GetWithDefault<VtDictionary>();
        dict[config_key] = value;
        plug->OnConfigChanged(dict);
        _config[plugin] = dict;
    }
}

VtDictionary UsdTweakPluginRegistry::GetConfig(const std::string &plugin) {
    if (auto plug = GetPluginByName(plugin)) {
        return _config[plugin].GetWithDefault<VtDictionary>();
    }
    return {};
}

VtValue UsdTweakPluginRegistry::GetConfig(const std::string &plugin, const std::string &config_key) {
    if (auto plug = GetPluginByName(plugin)) {
        auto dict_v = _config[plugin];
        if (!dict_v.IsEmpty()) {
            const VtDictionary &dict = dict_v.Get<VtDictionary>();
            return *dict.GetValueAtPath(config_key);
        }
    }
    return {};
}
