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

namespace {

struct Objc3IRModuleMetadataPublicationProgramView {
  const std::string &module_name;
};

}  // namespace

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
  const Objc3IRModuleMetadataPublicationProgramView program_{
      options.module_name};
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
    out << "; frontend_objc_object_pointer_nullability_generics_profile = object_pointer_type_spellings="
        << frontend_metadata_.object_pointer_type_spellings
        << ", pointer_declarator_entries=" << frontend_metadata_.pointer_declarator_entries
        << ", pointer_declarator_depth_total=" << frontend_metadata_.pointer_declarator_depth_total
        << ", pointer_declarator_token_entries=" << frontend_metadata_.pointer_declarator_token_entries
        << ", nullability_suffix_entries=" << frontend_metadata_.nullability_suffix_entries
        << ", generic_suffix_entries=" << frontend_metadata_.generic_suffix_entries
        << ", terminated_generic_suffix_entries=" << frontend_metadata_.terminated_generic_suffix_entries
        << ", unterminated_generic_suffix_entries=" << frontend_metadata_.unterminated_generic_suffix_entries
        << ", deterministic_object_pointer_nullability_generics_handoff="
        << (frontend_metadata_.deterministic_object_pointer_nullability_generics_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_symbol_graph_scope_resolution_profile = global_symbol_nodes="
        << frontend_metadata_.global_symbol_nodes
        << ", function_symbol_nodes=" << frontend_metadata_.function_symbol_nodes
        << ", interface_symbol_nodes=" << frontend_metadata_.interface_symbol_nodes
        << ", implementation_symbol_nodes=" << frontend_metadata_.implementation_symbol_nodes
        << ", interface_property_symbol_nodes=" << frontend_metadata_.interface_property_symbol_nodes
        << ", implementation_property_symbol_nodes=" << frontend_metadata_.implementation_property_symbol_nodes
        << ", interface_method_symbol_nodes=" << frontend_metadata_.interface_method_symbol_nodes
        << ", implementation_method_symbol_nodes=" << frontend_metadata_.implementation_method_symbol_nodes
        << ", top_level_scope_symbols=" << frontend_metadata_.top_level_scope_symbols
        << ", nested_scope_symbols=" << frontend_metadata_.nested_scope_symbols
        << ", scope_frames_total=" << frontend_metadata_.scope_frames_total
        << ", implementation_interface_resolution_sites="
        << frontend_metadata_.implementation_interface_resolution_sites
        << ", implementation_interface_resolution_hits="
        << frontend_metadata_.implementation_interface_resolution_hits
        << ", implementation_interface_resolution_misses="
        << frontend_metadata_.implementation_interface_resolution_misses
        << ", method_resolution_sites=" << frontend_metadata_.method_resolution_sites
        << ", method_resolution_hits=" << frontend_metadata_.method_resolution_hits
        << ", method_resolution_misses=" << frontend_metadata_.method_resolution_misses
        << ", deterministic_symbol_graph_handoff="
        << (frontend_metadata_.deterministic_symbol_graph_handoff ? "true" : "false")
        << ", deterministic_scope_resolution_handoff="
        << (frontend_metadata_.deterministic_scope_resolution_handoff ? "true" : "false")
        << ", deterministic_symbol_graph_scope_resolution_handoff_key="
        << frontend_metadata_.deterministic_symbol_graph_scope_resolution_handoff_key << "\n";
    out << "source_filename = \"" << program_.module_name << ".objc3\"\n\n";
}
