#include "io/objc3_cross_module_runtime_link_plan_ordering.h"

#include <algorithm>
#include <sstream>

std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
BuildOrderedObjc3CrossModuleRuntimeLinkPlanImportedInputs(
    const std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput>
        &imported_inputs) {
  std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput> ordered_inputs =
      imported_inputs;
  std::sort(ordered_inputs.begin(), ordered_inputs.end(),
            [](const auto &lhs, const auto &rhs) {
              if (lhs.translation_unit_registration_order_ordinal !=
                  rhs.translation_unit_registration_order_ordinal) {
                return lhs.translation_unit_registration_order_ordinal <
                       rhs.translation_unit_registration_order_ordinal;
              }
              if (lhs.translation_unit_identity_key !=
                  rhs.translation_unit_identity_key) {
                return lhs.translation_unit_identity_key <
                       rhs.translation_unit_identity_key;
              }
              return lhs.module_name < rhs.module_name;
            });
  return ordered_inputs;
}

std::vector<std::string>
BuildOrderedObjc3CrossModuleDirectImportSurfaceArtifactPaths(
    const std::vector<std::string> &direct_import_surface_artifact_paths) {
  std::vector<std::string> ordered_paths =
      direct_import_surface_artifact_paths;
  std::sort(ordered_paths.begin(), ordered_paths.end());
  return ordered_paths;
}

std::string BuildObjc3CrossModuleRuntimeLinkerResponsePayload(
    const std::vector<std::string> &merged_driver_linker_flags) {
  std::ostringstream response_out;
  for (const auto &flag : merged_driver_linker_flags) {
    response_out << flag << "\n";
  }
  return response_out.str();
}
