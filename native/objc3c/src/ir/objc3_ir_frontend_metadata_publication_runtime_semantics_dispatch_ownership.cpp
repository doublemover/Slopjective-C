#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership.h"

#include <cstddef>
#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRDispatchOwnershipMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  out << "!66 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_instance_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_class_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_super_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_direct_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_dynamic_sites)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_instance_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_class_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_super_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_direct_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_dynamic_entrypoint_family)
      << "\", i1 "
      << (metadata.deterministic_dispatch_surface_classification_handoff ? 1 : 0)
      << "}\n";
  out << "!67 = !{!\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_descriptor_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_offset_global_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_table_model)
      << "\", i1 " << (metadata.executable_ivar_layout_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.executable_ivar_layout_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_offset_global_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_layout_table_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_layout_owner_entries)
      << ", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_emission_replay_key)
      << "\"}\n";
  out << "!68 = !{!\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << "}\n";
  out << "!69 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionAccessorModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionPropertyContextModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionAutoreleaseModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << "}\n";
  out << "!70 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiReferenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << "}\n";
  out << "!71 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementImplementationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationRefcountModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementImplementationWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.autoreleasepool_scope_lowering_scope_sites)
      << "}\n";
  out << "!72 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateSupportedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateEvidenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationContractId)
      << "\"}\n";
}
