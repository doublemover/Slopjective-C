#include "io/objc3_cross_module_runtime_link_plan_document.h"

#include <sstream>
#include <vector>

#include "io/objc3_cross_module_imported_modules_document.h"
#include "io/objc3_cross_module_runtime_link_plan_document_artifacts.h"
#include "io/objc3_cross_module_runtime_link_plan_document_counts.h"
#include "io/objc3_cross_module_runtime_link_plan_document_header.h"
#include "io/objc3_cross_module_runtime_link_plan_inputs.h"
#include "io/objc3_cross_module_runtime_link_plan_ordering.h"
#include "io/objc3_cross_module_runtime_link_plan_sections.h"

bool BuildObjc3CrossModuleRuntimeLinkPlanDocument(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    std::string &plan_json,
    std::string &linker_response_payload,
    std::string &error) {
  plan_json.clear();
  linker_response_payload.clear();
  error.clear();

  if (!TryValidateObjc3CrossModuleRuntimeLinkPlanArtifactInputs(inputs,
                                                               error)) {
    return false;
  }

  const std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
      imported_inputs =
          BuildOrderedObjc3CrossModuleRuntimeLinkPlanImportedInputs(
              inputs.imported_inputs);

  Objc3CrossModuleRuntimeLinkPlanSections sections;
  if (!BuildObjc3CrossModuleRuntimeLinkPlanSections(inputs,
                                                    imported_inputs,
                                                    sections,
                                                    error)) {
    return false;
  }

  const std::string imported_modules_json =
      BuildObjc3CrossModuleImportedModulesJson(imported_inputs);

  std::ostringstream out;
  EmitObjc3CrossModuleRuntimeLinkPlanDocumentHeader(out, inputs, sections);
  EmitObjc3CrossModuleRuntimeLinkPlanDocumentCounts(
      out, inputs, sections, imported_inputs.size());
  EmitObjc3CrossModuleRuntimeLinkPlanDocumentArtifacts(
      out, inputs, sections, imported_modules_json);
  plan_json = out.str();

  linker_response_payload =
      BuildObjc3CrossModuleRuntimeLinkerResponsePayload(
          sections.merged_driver_linker_flags);
  return true;
}
