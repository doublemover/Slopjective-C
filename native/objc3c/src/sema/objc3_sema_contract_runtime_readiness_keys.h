#pragma once

#include <cstddef>
#include <string>

inline constexpr const char
    *kObjc3CrossModuleSemanticContractsDiagnosticsContractId =
        "objc3c.cross_module.semantic.contracts.diagnostics.closure.v1";
inline constexpr const char
    *kObjc3CrossModuleSemanticContractsDiagnosticsSurfacePath =
        "frontend.pipeline.semantic_surface.objc_cross_module_semantic_contracts_and_diagnostics";
inline constexpr const char
    *kObjc3CrossModuleSemanticContractsDiagnosticsRule =
        "module-import-namespace-api-partition-cache-invalidation-cross-module-conformance-and-diagnostic-recovery-packets-share-one-deterministic-semantic-contract-rooted-in-live-sema-and-lowering-surfaces";

struct Objc3CrossModuleSemanticContractsDiagnosticsSummary {
  std::string contract_id =
      kObjc3CrossModuleSemanticContractsDiagnosticsContractId;
  std::string surface_path =
      kObjc3CrossModuleSemanticContractsDiagnosticsSurfacePath;
  std::string semantic_model =
      kObjc3CrossModuleSemanticContractsDiagnosticsRule;
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t namespace_collision_shadowing_sites = 0;
  std::size_t public_private_api_partition_sites = 0;
  std::size_t incremental_module_cache_invalidation_sites = 0;
  std::size_t cross_module_conformance_sites = 0;
  std::size_t normalized_cross_module_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t diagnostic_recovery_sites = 0;
  std::size_t diagnostic_emit_sites = 0;
  std::size_t recovery_anchor_sites = 0;
  std::size_t recovery_boundary_sites = 0;
  std::size_t fail_closed_diagnostic_sites = 0;
  std::size_t diagnostic_normalized_sites = 0;
  std::size_t diagnostic_gate_blocked_sites = 0;
  std::size_t interop_import_module_annotation_sites = 0;
  std::size_t interop_imported_module_name_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool module_import_graph_semantics_landed = false;
  bool namespace_collision_semantics_landed = false;
  bool public_private_partition_semantics_landed = false;
  bool incremental_cache_semantics_landed = false;
  bool cross_module_conformance_semantics_landed = false;
  bool diagnostic_recovery_semantics_landed = false;
  bool interop_import_semantics_landed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};
