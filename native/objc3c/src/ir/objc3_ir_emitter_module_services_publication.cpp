#include "ir/objc3_ir_emitter_module_services_publication.h"

#include "ir/objc3_ir_emitter_service_contexts.h"
#include "ir/objc3_ir_module_metadata_publication.h"

Objc3IRModuleMetadataPublicationOptions
BuildObjc3IREmitterModuleMetadataPublicationOptions(
    const Objc3IREmitterServiceContextState &state) {
  return Objc3IRModuleMetadataPublicationOptions{
      state.program.module_name,
      state.frontend_metadata,
      state.lowering_ir_boundary,
      state.synthesized_property_accessor_count,
      state.vector_signature_function_count};
}
