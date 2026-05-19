#include "ir/objc3_ir_module_metadata_publication.h"

#include <cstddef>
#include <sstream>
#include <string>

#include "ir/objc3_ir_emission_readiness_publication.h"
#include "ir/objc3_ir_lowering_extension_metadata_publication.h"
#include "ir/objc3_ir_module_metadata_publication_advanced_profiles.h"
#include "ir/objc3_ir_module_metadata_publication_core_profiles.h"
#include "ir/objc3_ir_module_metadata_publication_lowering_profiles.h"
#include "ir/objc3_ir_module_metadata_publication_prelude.h"
#include "ir/objc3_ir_module_metadata_publication_runtime_gates.h"
#include "ir/objc3_ir_module_metadata_publication_tail.h"

void EmitObjc3IRModuleMetadataPublication(
    const Objc3IRModuleMetadataPublicationOptions &options,
    std::ostringstream &out) {
  const Objc3IRFrontendMetadata &frontend_metadata_ =
      options.frontend_metadata;
  const Objc3LoweringIRBoundary &lowering_ir_boundary_ =
      options.lowering_ir_boundary;
  const std::size_t synthesized_property_accessor_count_ =
      options.synthesized_property_accessor_count;
  const std::size_t vector_signature_function_count_ =
      options.vector_signature_function_count;
    EmitObjc3IRModuleMetadataPreludePublication(
        Objc3IRModuleMetadataPreludePublicationOptions{
            frontend_metadata_, lowering_ir_boundary_,
            synthesized_property_accessor_count_},
        out);
    EmitObjc3IRLoweringExtensionCommentPublication(frontend_metadata_, out);
    EmitObjc3IREmissionReadinessPublication(frontend_metadata_, out);
    out << "; simd_vector_function_signatures = " << vector_signature_function_count_ << "\n";
    EmitObjc3IRModuleMetadataCoreProfilePublication(
        frontend_metadata_, synthesized_property_accessor_count_, out);
    EmitObjc3IRModuleMetadataRuntimeSemanticsGatePublication(
        frontend_metadata_, out);
    EmitObjc3IRModuleMetadataLoweringProfilePublication(frontend_metadata_, out);
    EmitObjc3IRModuleMetadataAdvancedProfilePublication(frontend_metadata_, out);
    EmitObjc3IRModuleMetadataTailPublication(
        frontend_metadata_, options.module_name, out);
}
