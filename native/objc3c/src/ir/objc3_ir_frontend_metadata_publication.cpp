#include "ir/objc3_ir_frontend_metadata_publication.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_core.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "lower/objc3_lowering_contract.h"

std::string BuildObjc3IRFrontendProfileComment(
    const Objc3IRFrontendMetadata &metadata) {
  std::ostringstream out;
  out << "; frontend_profile = language_version="
      << static_cast<unsigned>(metadata.language_version)
      << ", language_profile=" << metadata.language_profile
      << ", arc_mode=" << metadata.arc_mode
      << ", canonical_literal_rejection_total="
      << metadata.canonical_literal_rejection_total();
  return out.str();
}

void EmitObjc3IRFrontendMetadataPublication(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  EmitObjc3IRFrontendCoreMetadataPublication(metadata, out);
  EmitObjc3IRRuntimeMetadataBoundaryNodes(metadata, out);
  Objc3RuntimeMetadataLayoutPolicy runtime_metadata_layout_policy;
  std::string runtime_metadata_layout_policy_error;
  if (!BuildObjc3IRRuntimeMetadataLayoutPolicy(
          metadata, runtime_metadata_layout_policy,
          runtime_metadata_layout_policy_error) &&
      runtime_metadata_layout_policy.failure_reason.empty()) {
    runtime_metadata_layout_policy.failure_reason =
        runtime_metadata_layout_policy_error;
  }
  EmitObjc3IRRuntimeSupportMetadataNodes(metadata, out);
  EmitObjc3IRRuntimeMetadataObjectPublicationNodes(
      metadata, runtime_metadata_layout_policy,
      runtime_metadata_symbols.linker_anchor_symbol,
      runtime_metadata_symbols.discovery_root_symbol, selector_pool_global_count,
      runtime_string_pool_global_count, out);
  EmitObjc3IRDispatchOwnershipMetadataNodes(
      metadata, synthesized_property_accessor_count, out);
  EmitObjc3IRBlockArcMetadataNodes(metadata, out);
  EmitObjc3IRErrorHandlingMetadataNodes(metadata, out);
  EmitObjc3IRConcurrencyRuntimeMetadataNodes(out);
  EmitObjc3IRTypeSymbolDispatchCounterNodes(metadata, out);
  EmitObjc3IRDispatchOwnershipLoweringCounterNodes(metadata, out);
  EmitObjc3IRBlockLoweringCounterNodes(metadata, out);
  EmitObjc3IRTypeModuleLoweringCounterNodes(metadata, out);
  EmitObjc3IRModuleGovernanceLoweringCounterNodes(metadata, out);
  EmitObjc3IRErrorHandlingLoweringCounterNodes(metadata, out);
  EmitObjc3IRSafetyConcurrencyLoweringCounterNodes(metadata, out);
  EmitObjc3IRActorDispatchControlMetadataNodes(metadata, out);
  EmitObjc3IRInteropLoweringMetadataNodes(metadata, out);
  EmitObjc3IRMetaprogrammingLoweringMetadataNodes(metadata, out);
  EmitObjc3IRDispatchMetadataPreservationNodes(metadata, out);
  EmitObjc3IROwnershipExtensionMetadataNodes(metadata, out);
  EmitObjc3IRAsyncDiagnosticLoweringCounterNodes(metadata, out);
}
