#include "ir/objc3_ir_module_metadata_publication_prelude.h"

#include <sstream>

#include "ir/objc3_ir_emission_prologue.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_module_metadata_publication_prelude_lowering_replay.h"
#include "ir/objc3_ir_property_metadata_comment_emission.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRModuleMetadataPreludePublication(
    const Objc3IRModuleMetadataPreludePublicationOptions &options,
    std::ostringstream &out) {
  const Objc3IRFrontendMetadata &frontend_metadata_ =
      options.frontend_metadata;
  const Objc3LoweringIRBoundary &lowering_ir_boundary_ =
      options.lowering_ir_boundary;
  const std::size_t synthesized_property_accessor_count_ =
      options.synthesized_property_accessor_count;
  out << BuildObjc3IRModulePrologue(Objc3IRModulePrologue{
      Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_),
      Objc3RuntimeDispatchDeclarationReplayKey(lowering_ir_boundary_),
      Objc3SimdVectorTypeLoweringReplayKey(),
  });
  EmitObjc3IRPropertyMetadataCommentEmission(
      Objc3IRPropertyMetadataCommentEmissionOptions{
          frontend_metadata_, lowering_ir_boundary_,
          synthesized_property_accessor_count_},
      out);
  if (!frontend_metadata_.runtime_metadata_source_ownership_contract_id.empty()) {
    out << "; runtime_metadata_source_ownership = "
        << frontend_metadata_.runtime_metadata_source_ownership_contract_id << "\n";
  }
  if (!frontend_metadata_.runtime_export_legality_contract_id.empty()) {
    out << "; runtime_export_legality = "
        << frontend_metadata_.runtime_export_legality_contract_id << "\n";
  }
  if (!frontend_metadata_.runtime_export_enforcement_contract_id.empty()) {
    out << "; runtime_export_enforcement = "
        << frontend_metadata_.runtime_export_enforcement_contract_id << "\n";
  }
  if (!frontend_metadata_.runtime_metadata_section_abi_contract_id.empty()) {
    out << "; runtime_metadata_section_abi = "
        << frontend_metadata_.runtime_metadata_section_abi_contract_id
        << "\n";
  }
  if (!frontend_metadata_.runtime_metadata_section_publication_contract_id.empty()) {
    out << "; runtime_metadata_section_publication = "
        << frontend_metadata_.runtime_metadata_section_publication_contract_id
        << "\n";
  }
  if (!frontend_metadata_.runtime_metadata_object_inspection_contract_id.empty()) {
    out << "; runtime_metadata_object_inspection = "
        << frontend_metadata_.runtime_metadata_object_inspection_contract_id
        << "\n";
  }
  out << "; runtime_selector_lookup_tables = contract="
      << kObjc3RuntimeSelectorLookupTablesContractId
      << ", interning_model="
      << kObjc3RuntimeSelectorLookupTablesInterningModel
      << ", merge_model="
      << kObjc3RuntimeSelectorLookupTablesMergeModel
      << ", dynamic_miss_model="
      << kObjc3RuntimeSelectorLookupTablesDynamicMissModel << "\n";
  out << "; runtime_method_cache_slow_path_lookup = contract="
      << kObjc3RuntimeMethodCacheSlowPathContractId
      << ", receiver_normalization_model="
      << kObjc3RuntimeMethodCacheSlowPathReceiverNormalizationModel
      << ", resolution_model="
      << kObjc3RuntimeMethodCacheSlowPathResolutionModel
      << ", cache_model="
      << kObjc3RuntimeMethodCacheSlowPathCacheModel
      << ", strict_error_model="
      << kObjc3RuntimeMethodCacheSlowPathStrictErrorModel << "\n";
  out << "; runtime_protocol_category_method_resolution = contract="
      << kObjc3RuntimeProtocolCategoryMethodResolutionContractId
      << ", category_model="
      << kObjc3RuntimeProtocolCategoryMethodResolutionCategoryModel
      << ", protocol_model="
      << kObjc3RuntimeProtocolCategoryMethodResolutionProtocolModel
      << ", strict_error_model="
      << kObjc3RuntimeProtocolCategoryMethodResolutionStrictErrorModel << "\n";
  if (!frontend_metadata_
           .executable_metadata_debug_projection_contract_id.empty()) {
    out << "; executable_metadata_debug_projection = "
        << frontend_metadata_.executable_metadata_debug_projection_contract_id
        << "\n";
  }
  if (!frontend_metadata_.runtime_support_library_contract_id.empty()) {
    out << "; runtime_support_library = "
        << frontend_metadata_.runtime_support_library_contract_id << "\n";
  }
  if (!frontend_metadata_.runtime_support_library_core_feature_contract_id
           .empty()) {
    out << "; runtime_support_library_core_feature = "
        << frontend_metadata_.runtime_support_library_core_feature_contract_id
        << "\n";
  }
  if (!frontend_metadata_.runtime_support_library_link_wiring_contract_id
           .empty()) {
    out << "; runtime_support_library_link_wiring = "
        << frontend_metadata_.runtime_support_library_link_wiring_contract_id
        << "\n";
  }
  EmitObjc3IRModuleMetadataPreludeLoweringReplayPublication(
      frontend_metadata_, out);
}
