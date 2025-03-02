#include "Editor.h"
#include "UsdTweakPluginRegistry.h"
#include <pxr/base/tf/instantiateType.h>
#include <pxr/base/tf/notice.h>

PXR_NAMESPACE_USING_DIRECTIVE

/*
  Implements an Imgui panel for viewing usdtweak plugins (those derived from UsdTweakPluginBase)
*/
class UsdTweakPluginPanel : public UsdTweakPluginBase {
  public:
    static TfRefPtr<UsdTweakPluginPanel> New();

    inline const std::string &GetName() override { return _name; }

    bool OnLoad(const VtDictionary &config) override;
    void OnConfigChanged(const VtDictionary &config) override;

    void DrawPanel(const EditorDrawTopLevelNotice &event);

  private:
    UsdTweakPluginPanel() = default;
    ~UsdTweakPluginPanel() = default;

    const std::string _name = "UsdTweakPluginPanel";
    bool _show = true;
};