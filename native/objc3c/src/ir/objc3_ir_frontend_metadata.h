#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "ir/objc3_ir_frontend_metadata_block.h"
#include "ir/objc3_ir_frontend_metadata_concurrency.h"
#include "ir/objc3_ir_frontend_metadata_dispatch.h"
#include "ir/objc3_ir_frontend_metadata_dispatch_support.h"
#include "ir/objc3_ir_frontend_metadata_error_handling.h"
#include "ir/objc3_ir_frontend_metadata_interop.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming.h"
#include "ir/objc3_ir_frontend_metadata_module_source_linkage.h"
#include "ir/objc3_ir_frontend_metadata_ownership.h"
#include "ir/objc3_ir_frontend_metadata_ownership_support.h"
#include "ir/objc3_ir_frontend_metadata_pipeline_readiness.h"
#include "ir/objc3_ir_frontend_metadata_runtime_bundles.h"
#include "ir/objc3_ir_frontend_metadata_runtime_metadata.h"
#include "ir/objc3_ir_frontend_metadata_runtime_support.h"
#include "ir/objc3_ir_frontend_metadata_semantic_surface.h"
#include "ir/objc3_ir_frontend_metadata_task_runtime_support.h"
#include "ir/objc3_ir_frontend_metadata_type_system.h"
#include "ir/objc3_ir_frontend_metadata_unsafe_intrinsics.h"
// Historical extraction contract marker:
// #include "parse/objc3_parser_contract.h"

struct Objc3Program;

struct Objc3IRFrontendMetadata : Objc3IRFrontendRuntimeSupportMetadata,
                                 Objc3IRFrontendPipelineReadinessMetadata,
                                 Objc3IRFrontendRuntimeMetadata,
                                 Objc3IRFrontendDispatchMetadata,
                                 Objc3IRFrontendOwnershipMetadata,
                                 Objc3IRFrontendBlockMetadata,
                                 Objc3IRFrontendTypeSystemMetadata,
                                 Objc3IRFrontendModuleSourceLinkageMetadata,
                                 Objc3IRFrontendErrorHandlingMetadata,
                                 Objc3IRFrontendSemanticSurfaceMetadata,
                                 Objc3IRFrontendConcurrencyMetadata,
                                 Objc3IRFrontendDispatchSupportMetadata,
                                 Objc3IRFrontendInteropMetadata,
                                 Objc3IRFrontendMetaprogrammingMetadata,
                                 Objc3IRFrontendOwnershipSupportMetadata,
                                 Objc3IRFrontendTaskRuntimeSupportMetadata,
                                 Objc3IRFrontendUnsafeIntrinsicsMetadata {
  std::uint8_t language_version = 3u;
  std::string language_profile = "canonical";
  std::string arc_mode = "disabled";
  bool arc_mode_enabled = false;
  bool versioned_conformance_report_lowering_ready = false;
  std::string versioned_conformance_report_lowering_replay_key;
  std::size_t canonical_literal_yes_rejection_sites = 0;
  std::size_t canonical_literal_no_rejection_sites = 0;
  std::size_t canonical_literal_null_rejection_sites = 0;
  std::size_t declared_interfaces = 0;
  std::size_t declared_implementations = 0;
  std::size_t resolved_interface_symbols = 0;
  std::size_t resolved_implementation_symbols = 0;
  std::size_t interface_method_symbols = 0;
  std::size_t implementation_method_symbols = 0;
  std::size_t linked_implementation_symbols = 0;
  bool deterministic_interface_implementation_handoff = false;
  std::size_t declared_protocols = 0;
  std::size_t declared_categories = 0;
  std::size_t resolved_protocol_symbols = 0;
  std::size_t resolved_category_symbols = 0;
  std::size_t protocol_method_symbols = 0;
  std::size_t category_method_symbols = 0;
  std::size_t linked_category_symbols = 0;
  bool deterministic_protocol_category_handoff = false;
  std::size_t declared_class_interfaces = 0;
  std::size_t declared_class_implementations = 0;
  std::size_t resolved_class_interfaces = 0;
  std::size_t resolved_class_implementations = 0;
  std::size_t linked_class_method_symbols = 0;
  std::size_t linked_category_method_symbols = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  bool deterministic_class_protocol_category_linking_handoff = false;
  std::size_t selector_method_declaration_entries = 0;
  std::size_t selector_normalized_method_declarations = 0;
  std::size_t selector_piece_entries = 0;
  std::size_t selector_piece_parameter_links = 0;
  bool deterministic_selector_normalization_handoff = false;
  std::size_t property_declaration_entries = 0;
  std::size_t property_attribute_entries = 0;
  std::size_t property_attribute_value_entries = 0;
  std::size_t property_accessor_modifier_entries = 0;
  std::size_t property_getter_selector_entries = 0;
  std::size_t property_setter_selector_entries = 0;
  bool deterministic_property_attribute_handoff = false;
  std::size_t canonical_literal_rejection_total() const {
    return canonical_literal_yes_rejection_sites +
           canonical_literal_no_rejection_sites +
           canonical_literal_null_rejection_sites;
  }
};

