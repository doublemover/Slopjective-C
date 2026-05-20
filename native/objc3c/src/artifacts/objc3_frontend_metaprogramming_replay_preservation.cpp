#include "artifacts/objc3_frontend_metaprogramming_semantic_artifacts.h"

#include <algorithm>
#include <sstream>
#include <string_view>
#include <vector>

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "io/json/json_writer.h"
#include "io/objc3_json.h"
#include "pipeline/objc3_runtime_import_surface.h"
#include "pipeline/results/compile_options.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

std::size_t CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle> &bundles,
    std::string_view owner_kind) {
  return static_cast<std::size_t>(std::count_if(
      bundles.begin(), bundles.end(),
      [owner_kind](const Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle &bundle) {
        return bundle.owner_kind == owner_kind;
      }));
}

}  // namespace

Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
BuildMetaprogrammingModuleInterfaceReplayPreservationSummary(
    const Objc3MetaprogrammingExpansionLoweringContract &expansion_contract,
    const std::string &expansion_replay_key,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract
        &synthesized_contract,
    const std::string &synthesized_replay_key,
    const std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
        &property_behavior_bundles,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary summary;
  summary.expansion_lowering_replay_key = expansion_replay_key;
  summary.synthesized_emission_replay_key = synthesized_replay_key;
  summary.local_derive_method_count =
      synthesized_contract.emitted_derive_method_sites;
  summary.local_macro_artifact_count =
      synthesized_contract.emitted_macro_artifact_sites;
  summary.local_interface_property_behavior_artifact_count =
      CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
          property_behavior_bundles, "class-interface");
  summary.local_implementation_property_behavior_artifact_count =
      CountMetaprogrammingPropertyBehaviorArtifactBundlesByOwnerKind(
          property_behavior_bundles, "class-implementation");
  summary.local_runtime_method_list_count =
      synthesized_contract.emitted_runtime_method_list_sites;
  summary.runtime_import_artifact_ready =
      runtime_import_artifact_ready &&
      IsValidObjc3MetaprogrammingExpansionLoweringContract(expansion_contract) &&
      IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(
          synthesized_contract);
  summary.deterministic =
      expansion_contract.deterministic && synthesized_contract.deterministic;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.metaprogramming_module_interface_replay_preservation_present) {
      continue;
    }
    ++summary.imported_module_count;
    if (!surface.frontend_closure_summary.module_name.empty()) {
      summary.imported_module_names_lexicographic.push_back(
          surface.frontend_closure_summary.module_name);
    }
    summary.imported_derive_method_count +=
        surface.metaprogramming_local_derive_method_count;
    summary.imported_macro_artifact_count +=
        surface.metaprogramming_local_macro_artifact_count;
    summary.imported_interface_property_behavior_artifact_count +=
        surface.metaprogramming_local_interface_property_behavior_artifact_count;
    summary.imported_implementation_property_behavior_artifact_count +=
        surface.metaprogramming_local_implementation_property_behavior_artifact_count;
    summary.imported_runtime_method_list_count +=
        surface.metaprogramming_local_runtime_method_list_count;
    summary.deterministic =
        summary.deterministic && surface.metaprogramming_deterministic;
  }
  std::sort(summary.imported_module_names_lexicographic.begin(),
            summary.imported_module_names_lexicographic.end());
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready &&
      summary.imported_module_names_lexicographic.size() ==
          summary.imported_module_count;
  std::ostringstream replay_key;
  replay_key << Objc3MetaprogrammingModuleInterfaceReplayPreservationSummary()
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_preservation_ready="
             << (summary.separate_compilation_preservation_ready ? "true"
                                                                 : "false")
             << ";imported_module_count=" << summary.imported_module_count
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";expansion_lowering_replay_key=" << expansion_replay_key
             << ";synthesized_emission_replay_key=" << synthesized_replay_key
             << ";local_derive_method_count="
             << summary.local_derive_method_count
             << ";local_macro_artifact_count="
             << summary.local_macro_artifact_count
             << ";local_interface_property_behavior_artifact_count="
             << summary.local_interface_property_behavior_artifact_count
             << ";local_implementation_property_behavior_artifact_count="
             << summary.local_implementation_property_behavior_artifact_count
             << ";local_runtime_method_list_count="
             << summary.local_runtime_method_list_count
             << ";imported_derive_method_count="
             << summary.imported_derive_method_count
             << ";imported_macro_artifact_count="
             << summary.imported_macro_artifact_count
             << ";imported_interface_property_behavior_artifact_count="
             << summary.imported_interface_property_behavior_artifact_count
             << ";imported_implementation_property_behavior_artifact_count="
             << summary.imported_implementation_property_behavior_artifact_count
             << ";imported_runtime_method_list_count="
             << summary.imported_runtime_method_list_count;
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildMetaprogrammingModuleInterfaceReplayPreservationSummaryJson(
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"expansion_lowering_replay_key\":\""
      << EscapeJsonString(summary.expansion_lowering_replay_key)
      << "\",\"synthesized_emission_replay_key\":\""
      << EscapeJsonString(summary.synthesized_emission_replay_key)
      << "\",\"imported_module_names_lexicographic\":"
      << BuildStringArrayJson(summary.imported_module_names_lexicographic)
      << ",\"local_derive_method_count\":"
      << summary.local_derive_method_count
      << ",\"local_macro_artifact_count\":"
      << summary.local_macro_artifact_count
      << ",\"local_interface_property_behavior_artifact_count\":"
      << summary.local_interface_property_behavior_artifact_count
      << ",\"local_implementation_property_behavior_artifact_count\":"
      << summary.local_implementation_property_behavior_artifact_count
      << ",\"local_runtime_method_list_count\":"
      << summary.local_runtime_method_list_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"imported_derive_method_count\":"
      << summary.imported_derive_method_count
      << ",\"imported_macro_artifact_count\":"
      << summary.imported_macro_artifact_count
      << ",\"imported_interface_property_behavior_artifact_count\":"
      << summary.imported_interface_property_behavior_artifact_count
      << ",\"imported_implementation_property_behavior_artifact_count\":"
      << summary.imported_implementation_property_behavior_artifact_count
      << ",\"imported_runtime_method_list_count\":"
      << summary.imported_runtime_method_list_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary
BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary(
    const Objc3MetaprogrammingModuleInterfaceReplayPreservationSurfaceSummary
        &module_interface_summary,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces,
    const Objc3FrontendOptions &options) {
  Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary summary;
  if (!options.metaprogramming_cache_root_relative_path.empty()) {
    summary.cache_root_relative_path =
        options.metaprogramming_cache_root_relative_path;
  }
  summary.metaprogramming_replay_key = module_interface_summary.replay_key;
  summary.local_macro_artifact_count =
      module_interface_summary.local_macro_artifact_count;
  summary.local_property_behavior_artifact_count =
      module_interface_summary.local_interface_property_behavior_artifact_count +
      module_interface_summary
          .local_implementation_property_behavior_artifact_count;
  summary.runtime_import_artifact_ready =
      module_interface_summary.runtime_import_artifact_ready;
  summary.deterministic = module_interface_summary.deterministic;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.metaprogramming_macro_host_process_cache_runtime_integration_present) {
      continue;
    }
    ++summary.imported_module_count;
    summary.deterministic =
        summary.deterministic &&
        surface.metaprogramming_macro_host_process_cache_deterministic;
  }
  summary.separate_compilation_ready =
      summary.runtime_import_artifact_ready &&
      module_interface_summary.separate_compilation_preservation_ready;
  std::ostringstream replay_key;
  replay_key << Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary()
             << ";metaprogramming_replay_key=" << module_interface_summary.replay_key
             << ";local_macro_artifact_count="
             << summary.local_macro_artifact_count
             << ";local_property_behavior_artifact_count="
             << summary.local_property_behavior_artifact_count
             << ";imported_module_count=" << summary.imported_module_count
             << ";cache_root_relative_path=" << summary.cache_root_relative_path
             << ";invalidation_model=" << summary.invalidation_model
             << ";sandbox_policy_model=" << summary.sandbox_policy_model
             << ";diagnostics_model=" << summary.diagnostics_model
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_ready="
             << (summary.separate_compilation_ready ? "true" : "false")
             << ";deterministic="
             << (summary.deterministic ? "true" : "false");
  summary.replay_key = replay_key.str();
  return summary;
}

std::string BuildMetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummaryJson(
    const Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfaceSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"host_executable_relative_path\":\""
      << EscapeJsonString(summary.host_executable_relative_path)
      << "\",\"cache_root_relative_path\":\""
      << EscapeJsonString(summary.cache_root_relative_path)
      << "\",\"host_model\":\"" << EscapeJsonString(summary.host_model)
      << "\",\"toolchain_model\":\""
      << EscapeJsonString(summary.toolchain_model)
      << "\",\"cache_model\":\"" << EscapeJsonString(summary.cache_model)
      << "\",\"invalidation_model\":\""
      << EscapeJsonString(summary.invalidation_model)
      << "\",\"sandbox_policy_model\":\""
      << EscapeJsonString(summary.sandbox_policy_model)
      << "\",\"diagnostics_model\":\""
      << EscapeJsonString(summary.diagnostics_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"metaprogramming_replay_key\":\""
      << EscapeJsonString(summary.metaprogramming_replay_key)
      << "\",\"local_macro_artifact_count\":"
      << summary.local_macro_artifact_count
      << ",\"local_property_behavior_artifact_count\":"
      << summary.local_property_behavior_artifact_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_ready\":"
      << (summary.separate_compilation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
