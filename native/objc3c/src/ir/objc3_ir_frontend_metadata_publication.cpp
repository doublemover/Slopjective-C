#include "ir/objc3_ir_frontend_metadata_publication.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_emitter_groups.h"

void EmitObjc3IRFrontendMetadataPublication(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  EmitObjc3IRFrontendCorePublicationGroup(metadata, out);
  EmitObjc3IRFrontendRuntimePublicationGroup(
      metadata, runtime_metadata_symbols, selector_pool_global_count,
      runtime_string_pool_global_count, out);
  EmitObjc3IRFrontendRuntimeSemanticsPublicationGroup(
      metadata, synthesized_property_accessor_count, out);
  EmitObjc3IRFrontendLoweringPublicationGroup(metadata, out);
  EmitObjc3IRFrontendExtensionPublicationGroup(metadata, out);
}
