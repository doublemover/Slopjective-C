#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "sema/model/semantic_symbol_advanced_source_contracts.h"
#include "sema/model/semantic_symbol_core_source_closures.h"
#include "sema/model/semantic_symbol_metaprogramming_interop_summaries.h"
#include "sema/model/semantic_symbol_ownership_dispatch_summaries.h"

struct Objc3FrontendToolingDiagnosticsMigratorSourceInventorySummary {
  std::string contract_id =
      kObjc3ToolingDiagnosticsMigratorSourceInventoryContractId;
  std::string frontend_surface_path =
      kObjc3ToolingDiagnosticsMigratorSourceInventorySurfacePath;
  std::string source_model =
      kObjc3ToolingDiagnosticsMigratorSourceInventorySourceModel;
  std::string failure_model =
      kObjc3ToolingDiagnosticsMigratorSourceInventoryFailureModel;
  std::vector<std::string> dependency_contract_ids = {
      kObjc3ErrorHandlingErrorSourceClosureContractId,
      kObjc3ConcurrencyAsyncSourceClosureContractId,
      kObjc3ActorMemberIsolationSourceClosureContractId,
      kObjc3ConcurrencyTaskGroupCancellationSourceClosureContractId,
      kObjc3OwnershipSystemExtensionSourceClosureContractId,
      kObjc3OwnershipCleanupResourceCaptureSurfaceCompletionContractId,
      kObjc3OwnershipRetainableCFamilySourceCompletionContractId,
      kObjc3DispatchDispatchIntentSourceClosureContractId,
      kObjc3DispatchDispatchIntentSourceCompletionContractId,
      kObjc3MetaprogrammingMetaprogrammingSourceClosureContractId,
      kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionContractId,
      kObjc3MetaprogrammingPropertyBehaviorSourceCompletionContractId,
      kObjc3InteropForeignImportSourceClosureContractId,
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionContractId,
  };
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimAdvancedDiagnosticInventory,
      kObjc3SourceOnlyFeatureClaimAdvancedFixItInventory,
      kObjc3SourceOnlyFeatureClaimAdvancedMigratorInventory,
  };
  std::size_t advanced_feature_family_count = 0;
  std::size_t dependency_surface_count = 0;
  std::size_t aggregated_source_only_claim_count = 0;
  std::size_t fail_closed_construct_count = 0;
  std::size_t diagnostic_surface_sites = 0;
  std::size_t fixit_surface_sites = 0;
  std::size_t migrator_surface_sites = 0;
  std::size_t canonicalization_hint_sites = 0;
  std::size_t error_surface_sites = 0;
  std::size_t concurrency_surface_sites = 0;
  std::size_t system_surface_sites = 0;
  std::size_t dispatch_surface_sites = 0;
  std::size_t metaprogramming_surface_sites = 0;
  std::size_t interop_surface_sites = 0;
  bool diagnostics_inventory_source_supported = false;
  bool fixit_inventory_source_supported = false;
  bool migrator_inventory_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendToolingMigrationCanonicalizationSourceCompletionSummary {
  std::string contract_id =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionContractId;
  std::string dependency_contract_id =
      kObjc3ToolingDiagnosticsMigratorSourceInventoryContractId;
  std::string frontend_surface_path =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionSourceModel;
  std::string completion_model =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionCompletionModel;
  std::string failure_model =
      kObjc3ToolingMigrationCanonicalizationSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimAdvancedCanonicalizationInventory,
      kObjc3SourceOnlyFeatureClaimAdvancedMigrationAssistFlow,
  };
  std::string language_profile = "canonical";
  bool canonical_literal_rejection_diagnostics_enabled = false;
  bool canonical_literal_rejection_diagnostics_required = true;
  std::size_t legacy_yes_sites = 0;
  std::size_t legacy_no_sites = 0;
  std::size_t legacy_null_sites = 0;
  std::size_t legacy_total_sites = 0;
  std::size_t canonical_true_rewrite_sites = 0;
  std::size_t canonical_false_rewrite_sites = 0;
  std::size_t canonical_nil_rewrite_sites = 0;
  std::size_t canonicalization_candidate_sites = 0;
  std::size_t fixit_candidate_sites = 0;
  std::size_t migrator_candidate_sites = 0;
  bool dependency_inventory_ready = false;
  bool canonicalization_surface_supported = false;
  bool fixit_migration_surface_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};
