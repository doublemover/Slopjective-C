#pragma once

#include <cstddef>
#include <string>

#include "ast/objc3_ast_contracts_metadata_packaging.h"
#include "ast/objc3_ast_contracts_source_property_metadata.h"

struct Objc3RuntimeMetadataSectionAbiFreezeSummary {
  std::string contract_id = kObjc3RuntimeMetadataSectionAbiContractId;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool object_file_section_inventory_frozen = false;
  bool symbol_policy_frozen = false;
  bool visibility_model_frozen = false;
  bool retention_policy_frozen = false;
  bool runtime_metadata_source_boundary_ready = false;
  bool runtime_export_legality_boundary_ready = false;
  bool runtime_export_enforcement_ready = false;
  bool ready_for_section_scaffold = false;
  std::string logical_image_info_section =
      kObjc3RuntimeMetadataLogicalImageInfoSection;
  std::string logical_class_descriptor_section =
      kObjc3RuntimeMetadataLogicalClassDescriptorSection;
  std::string logical_protocol_descriptor_section =
      kObjc3RuntimeMetadataLogicalProtocolDescriptorSection;
  std::string logical_category_descriptor_section =
      kObjc3RuntimeMetadataLogicalCategoryDescriptorSection;
  std::string logical_property_descriptor_section =
      kObjc3RuntimeMetadataLogicalPropertyDescriptorSection;
  std::string logical_ivar_descriptor_section =
      kObjc3RuntimeMetadataLogicalIvarDescriptorSection;
  std::string descriptor_symbol_prefix =
      kObjc3RuntimeMetadataDescriptorSymbolPrefix;
  std::string aggregate_symbol_prefix =
      kObjc3RuntimeMetadataAggregateSymbolPrefix;
  std::string image_info_symbol = kObjc3RuntimeMetadataImageInfoSymbol;
  std::string descriptor_linkage =
      kObjc3RuntimeMetadataDescriptorLinkagePolicy;
  std::string aggregate_linkage =
      kObjc3RuntimeMetadataAggregateLinkagePolicy;
  std::string metadata_visibility = kObjc3RuntimeMetadataVisibilityPolicy;
  std::string retention_root = kObjc3RuntimeMetadataRetentionPolicyRoot;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary &summary) {
  return !summary.contract_id.empty() &&
         summary.boundary_frozen &&
         summary.fail_closed &&
         summary.object_file_section_inventory_frozen &&
         summary.symbol_policy_frozen &&
         summary.visibility_model_frozen &&
         summary.retention_policy_frozen &&
         summary.runtime_metadata_source_boundary_ready &&
         summary.runtime_export_legality_boundary_ready &&
         summary.runtime_export_enforcement_ready &&
         summary.ready_for_section_scaffold &&
         !summary.logical_image_info_section.empty() &&
         !summary.logical_class_descriptor_section.empty() &&
         !summary.logical_protocol_descriptor_section.empty() &&
         !summary.logical_category_descriptor_section.empty() &&
         !summary.logical_property_descriptor_section.empty() &&
         !summary.logical_ivar_descriptor_section.empty() &&
         !summary.descriptor_symbol_prefix.empty() &&
         !summary.aggregate_symbol_prefix.empty() &&
         !summary.image_info_symbol.empty() &&
         !summary.descriptor_linkage.empty() &&
         !summary.aggregate_linkage.empty() &&
         !summary.metadata_visibility.empty() &&
         !summary.retention_root.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeMetadataSectionPublicationSummary {
  std::string contract_id = kObjc3RuntimeMetadataSectionPublicationContractId;
  std::string abi_contract_id = kObjc3RuntimeMetadataSectionAbiContractId;
  bool publication_emitted = false;
  bool fail_closed = false;
  bool uses_llvm_used = false;
  bool image_info_emitted = false;
  std::size_t class_descriptor_count = 0;
  std::size_t protocol_descriptor_count = 0;
  std::size_t category_descriptor_count = 0;
  std::size_t property_descriptor_count = 0;
  std::size_t ivar_descriptor_count = 0;
  std::size_t total_descriptor_count = 0;
  std::size_t total_retained_global_count = 0;
  std::string image_info_symbol = kObjc3RuntimeMetadataImageInfoSymbol;
  std::string class_aggregate_symbol =
      kObjc3RuntimeMetadataClassDescriptorAggregateSymbol;
  std::string protocol_aggregate_symbol =
      kObjc3RuntimeMetadataProtocolDescriptorAggregateSymbol;
  std::string category_aggregate_symbol =
      kObjc3RuntimeMetadataCategoryDescriptorAggregateSymbol;
  std::string property_aggregate_symbol =
      kObjc3RuntimeMetadataPropertyDescriptorAggregateSymbol;
  std::string ivar_aggregate_symbol =
      kObjc3RuntimeMetadataIvarDescriptorAggregateSymbol;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
    const Objc3RuntimeMetadataSectionPublicationSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.abi_contract_id.empty() &&
         summary.publication_emitted &&
         summary.fail_closed &&
         summary.uses_llvm_used &&
         summary.image_info_emitted &&
         summary.total_descriptor_count ==
             summary.class_descriptor_count +
                 summary.protocol_descriptor_count +
                 summary.category_descriptor_count +
                 summary.property_descriptor_count +
                 summary.ivar_descriptor_count &&
         summary.total_retained_global_count ==
             summary.total_descriptor_count + 6u &&
         !summary.image_info_symbol.empty() &&
         !summary.class_aggregate_symbol.empty() &&
         !summary.protocol_aggregate_symbol.empty() &&
         !summary.category_aggregate_symbol.empty() &&
         !summary.property_aggregate_symbol.empty() &&
         !summary.ivar_aggregate_symbol.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeMetadataObjectInspectionHarnessSummary {
  std::string contract_id = kObjc3RuntimeMetadataObjectInspectionContractId;
  std::string publication_contract_id = kObjc3RuntimeMetadataSectionPublicationContractId;
  bool matrix_published = false;
  bool fail_closed = false;
  bool uses_llvm_readobj = false;
  bool uses_llvm_objdump = false;
  std::size_t matrix_row_count = 0;
  std::string fixture_path = kObjc3RuntimeMetadataObjectInspectionFixturePath;
  std::string emit_prefix = kObjc3RuntimeMetadataObjectInspectionEmitPrefix;
  std::string object_relative_path =
      kObjc3RuntimeMetadataObjectInspectionObjectRelativePath;
  std::string section_inventory_row_key =
      kObjc3RuntimeMetadataObjectInspectionSectionInventoryRowKey;
  std::string section_inventory_command =
      kObjc3RuntimeMetadataObjectInspectionSectionCommand;
  std::string symbol_inventory_row_key =
      kObjc3RuntimeMetadataObjectInspectionSymbolInventoryRowKey;
  std::string symbol_inventory_command =
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeMetadataObjectInspectionHarnessSummary(
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.publication_contract_id.empty() &&
         summary.matrix_published &&
         summary.fail_closed &&
         summary.uses_llvm_readobj &&
         summary.uses_llvm_objdump &&
         summary.matrix_row_count == 2u &&
         !summary.fixture_path.empty() &&
         !summary.emit_prefix.empty() &&
         !summary.object_relative_path.empty() &&
         !summary.section_inventory_row_key.empty() &&
         !summary.section_inventory_command.empty() &&
         !summary.symbol_inventory_row_key.empty() &&
         !summary.symbol_inventory_command.empty() &&
         summary.failure_reason.empty();
}
