#include "Editor.h"
#include "UsdTweakPluginRegistry.h"
#include <pxr/base/tf/instantiateType.h>
#include <pxr/base/tf/notice.h>
#include <pxr/base/arch/regex.h>
#include <pxr/usd/usd/prim.h>

PXR_NAMESPACE_USING_DIRECTIVE

class PrimFinder : public UsdTweakPluginBase {
  public:
    static TfRefPtr<PrimFinder> New();

    inline const std::string &GetName() override { return _name; }

    bool OnLoad(const VtDictionary &config) override;
    void OnConfigChanged(const VtDictionary &config) override;

    void FindPrims(const ArchRegex& regex);

    void DrawUI(const EditorDrawTopLevelNotice&);

  private:
    PrimFinder() = default;
    ~PrimFinder() = default;

    void ActivateResult(const UsdPrim& prim);

    const std::string _name = "PrimFinder";
    bool _enabled = true; 

    std::string _pattern;
    std::vector<UsdPrim> _results;
};