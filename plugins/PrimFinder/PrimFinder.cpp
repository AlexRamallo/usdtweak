#include "PrimFinder.h"

#include "Editor.h"
#include "viewport/Viewport.h"

#include <pxr/base/plug/registry.h>
#include <pxr/base/trace/trace.h>
#include <pxr/usd/usd/primRange.h>

#define WindowTitle "Prim Finder"

TF_INSTANTIATE_TYPE(PrimFinder, TfType::CONCRETE, TF_1_PARENT(UsdTweakPluginBase));
TF_REGISTRY_FUNCTION_WITH_TAG(UsdTweakPluginRegistry, PrimFinder) {
    UsdTweakPluginRegistry::GetInstance().Add(PrimFinder::New());
}

TfRefPtr<PrimFinder> PrimFinder::New() { return TfCreateRefPtr(new PrimFinder()); }

bool PrimFinder::OnLoad(const VtDictionary &config) {
    TfWeakPtr<PrimFinder> me(this);
    TfNotice::Register(me, &PrimFinder::DrawUI);
    return true;
}

void PrimFinder::OnConfigChanged(const VtDictionary &config) {
    _enabled = config.GetValueAtPath("enabled")->GetWithDefault<bool>(true);
}

void PrimFinder::FindPrims(const ArchRegex& regex)
{
    _results.clear();
    Editor* editor = UsdTweakPluginRegistry::GetInstance().GetEditor();
    for(UsdPrim prim : editor->GetCurrentStage()->TraverseAll())
    {
        if(regex.Match(prim.GetPath().GetString())){
            _results.push_back(prim);
        }
    }
}

void PrimFinder::DrawUI(const EditorDrawTopLevelNotice& ev)
{
    if(!_enabled){
        return;
    }
    static bool bad_regex = false;
    if(ImGui::Begin(WindowTitle)){
        if(ImGui::InputText("Regex", &_pattern))
        {
            ArchRegex regex(_pattern);
            if(!regex){
                bad_regex = true;
            }else{
                bad_regex = false;
                FindPrims(_pattern);
            }
        }
        if(bad_regex)
        {
            ImGui::TextColored({255,0,0,255}, "Invalid regex pattern");
        }


        ImVec2 listBoxSize(-FLT_MIN, -10);
        if (ImGui::BeginListBox("Results", listBoxSize)) {
            for (const auto &prim : _results) {
                if(ImGui::TextLink(prim.GetPath().GetString().c_str()))
                {
                    ActivateResult(prim);
                }
            }
            ImGui::EndListBox();
        }

        ImGui::End();
    }
}

void PrimFinder::ActivateResult(const UsdPrim& prim)
{
    Editor* editor = UsdTweakPluginRegistry::GetInstance().GetEditor();
    Viewport& vp = editor->GetViewport();
    Selection &selection = vp.GetSelection();
    auto owner = editor->GetCurrentStage();
    selection.SetSelected(owner, prim.GetPath());
    vp.FrameCameraOnSelection(selection);
}