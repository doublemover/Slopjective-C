#include "ir/objc3_ir_frontend_metadata_publication_emitter_groups.h"

#include <sstream>
#include <string>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_object.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "lower/contracts/runtime_metadata_layout_policy_record_contracts.h"

namespace {

Objc3RuntimeMetadataLayoutPolicy BuildObjc3IRFrontendRuntimeLayoutPolicy(
    const Objc3IRFrontendMetadata &metadata) {
  Objc3RuntimeMetadataLayoutPolicy runtime_metadata_layout_policy;
  std::string runtime_metadata_layout_policy_error;
  if (!BuildObjc3IRRuntimeMetadataLayoutPolicy(
          metadata, runtime_metadata_layout_policy,
          runtime_metadata_layout_policy_error) &&
      runtime_metadata_layout_policy.failure_reason.empty()) {
    runtime_metadata_layout_policy.failure_reason =
        runtime_metadata_layout_policy_error;
  }
  return runtime_metadata_layout_policy;
}

}  // namespace

void EmitObjc3IRFrontendRuntimePublicationGroup(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count, std::ostringstream &out) {
  EmitObjc3IRRuntimeMetadataBoundaryNodes(metadata, out);
  const Objc3RuntimeMetadataLayoutPolicy runtime_metadata_layout_policy =
      BuildObjc3IRFrontendRuntimeLayoutPolicy(metadata);
  EmitObjc3IRRuntimeSupportMetadataNodes(metadata, out);
  EmitObjc3IRRuntimeMetadataObjectPublicationNodes(
      metadata, runtime_metadata_layout_policy,
      runtime_metadata_symbols.linker_anchor_symbol,
      runtime_metadata_symbols.discovery_root_symbol, selector_pool_global_count,
      runtime_string_pool_global_count, out);
}
