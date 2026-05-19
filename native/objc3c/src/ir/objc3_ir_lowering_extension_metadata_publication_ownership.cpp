#include "ir/objc3_ir_lowering_extension_metadata_publication_ownership.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IROwnershipExtensionMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!98 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_cleanup_hook_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_resource_local_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_cleanup_owned_local_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_resource_move_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_borrowed_parameter_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_borrowed_return_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_borrowed_escape_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_explicit_capture_item_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_retainable_family_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .ownership_system_extension_lowering_retainable_family_operation_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .ownership_system_extension_lowering_retainable_family_alias_callable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_guard_blocked_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_system_extension_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_ownership_system_extension_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!99 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.ownership_borrowed_retainable_abi_completion_replay_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_returns_borrowed_attribute_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_family_retain_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_family_release_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_family_autorelease_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_compatibility_returns_retained_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .ownership_borrowed_retainable_compatibility_returns_not_retained_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_borrowed_retainable_compatibility_consumed_sites)
      << ", i1 "
      << (metadata
                  .deterministic_ownership_borrowed_retainable_abi_completion_handoff
              ? 1
              : 0)
      << "}\n\n";
  out << "!100 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipSystemHelperRuntimeContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipSystemExtensionLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\"objc3_runtime_copy_memory_management_state_for_testing\", !\"objc3_runtime_copy_arc_debug_state_for_testing\"}\n\n";
  out << "!101 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipLiveCleanupRetainableIntegrationContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipSystemHelperRuntimeContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\"objc3_runtime_copy_memory_management_state_for_testing\", !\"objc3_runtime_copy_arc_debug_state_for_testing\"}\n\n";
}
