#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::interop {

struct Objc3InteropBridgeCallableParameter {
  std::string type;
  std::string name;
};

struct Objc3InteropBridgeCallableArtifact {
  std::string name;
  std::string return_type;
  std::vector<Objc3InteropBridgeCallableParameter> parameters;
  std::string objc_import_module_name;
  bool objc_import_module_declared = false;
  std::string objc_header_name;
  bool objc_header_name_declared = false;
  std::string objc_cxx_name;
  bool objc_cxx_name_declared = false;
  std::string objc_swift_name;
  bool objc_swift_name_declared = false;
  std::string objc_export_header_name;
  bool objc_export_header_declared = false;
  std::size_t objc_abi_alignment_bytes = 0;
  bool objc_abi_align_declared = false;
  std::string objc_foreign_type_name;
  bool objc_foreign_type_declared = false;
  std::string objc_mixed_image_name;
  bool objc_mixed_image_declared = false;
  std::string objc_package_entry_name;
  bool objc_package_entry_declared = false;
};

struct Objc3InteropBridgeArtifactInputs {
  std::string contract_id;
  std::string module_name;
  std::string header_artifact_relative_path;
  std::string module_artifact_relative_path;
  std::string bridge_artifact_relative_path;
  bool runtime_generation_ready = false;
  bool cross_module_packaging_ready = false;
  bool deterministic = false;
  std::string replay_key;
  std::vector<std::string> local_import_module_names_lexicographic;
  std::vector<Objc3InteropBridgeCallableArtifact> foreign_callables;
};

[[nodiscard]] std::string BuildInteropBridgeHeaderArtifactText(
    const Objc3InteropBridgeArtifactInputs &inputs);

[[nodiscard]] std::string BuildInteropBridgeModuleArtifactText(
    const Objc3InteropBridgeArtifactInputs &inputs);

[[nodiscard]] std::string BuildInteropBridgeArtifactJson(
    const Objc3InteropBridgeArtifactInputs &inputs);

[[nodiscard]] std::string BuildInteropBridgeHeaderArtifactText(
    const Objc3Program &program,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary &bridge_summary);

[[nodiscard]] std::string BuildInteropBridgeModuleArtifactText(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary &bridge_summary);

[[nodiscard]] std::string BuildInteropBridgeArtifactJson(
    const Objc3Program &program,
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3InteropHeaderModuleBridgeGenerationSummary &bridge_summary);

}  // namespace objc3::artifacts::interop
