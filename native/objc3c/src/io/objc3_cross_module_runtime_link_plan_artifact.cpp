#include "io/objc3_cross_module_runtime_link_plan_document.h"

bool TryBuildObjc3CrossModuleRuntimeLinkPlanArtifact(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    std::string &plan_json,
    std::string &linker_response_payload,
    std::string &error) {
  return BuildObjc3CrossModuleRuntimeLinkPlanDocument(
      inputs, plan_json, linker_response_payload, error);
}
