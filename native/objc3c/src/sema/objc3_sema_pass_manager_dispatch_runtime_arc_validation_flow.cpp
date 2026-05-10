#include "sema/objc3_sema_pass_manager_contract_flow.h"

#include "objc3_sema_pass_manager_dispatch_runtime_arc_validation_flow_record_seed.inc"
#include "objc3_sema_pass_manager_dispatch_runtime_arc_validation_flow_dispatch_readiness.inc"
#include "objc3_sema_pass_manager_dispatch_runtime_arc_validation_flow_runtime_link_readiness.inc"
#include "objc3_sema_pass_manager_dispatch_runtime_arc_validation_flow_arc_ownership_readiness.inc"
#include "objc3_sema_pass_manager_dispatch_runtime_arc_validation_flow_arc_diagnostics_scope_readiness.inc"
#include "objc3_sema_pass_manager_dispatch_runtime_arc_validation_flow_validation_publication.inc"

Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord
BuildObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_dispatch_abi_marshalling_handoff,
    bool deterministic_nil_receiver_semantics_foldability_handoff,
    bool deterministic_super_dispatch_method_family_handoff,
    bool deterministic_runtime_link_host_link_handoff,
    bool deterministic_retain_release_operation_handoff,
    bool deterministic_weak_unowned_semantics_handoff,
    bool deterministic_arc_diagnostics_fixit_handoff,
    bool deterministic_autoreleasepool_scope_handoff) {
  Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord record =
      BuildObjc3SemaDispatchRuntimeArcValidationFlowRecordSeed(input);
  PopulateObjc3SemaDispatchRuntimeArcValidationFlowDispatchReadiness(
      record, surface, deterministic_dispatch_abi_marshalling_handoff,
      deterministic_nil_receiver_semantics_foldability_handoff,
      deterministic_super_dispatch_method_family_handoff);
  PopulateObjc3SemaDispatchRuntimeArcValidationFlowRuntimeLinkReadiness(
      record, surface, deterministic_runtime_link_host_link_handoff);
  PopulateObjc3SemaDispatchRuntimeArcValidationFlowArcOwnershipReadiness(
      record, surface, deterministic_retain_release_operation_handoff,
      deterministic_weak_unowned_semantics_handoff);
  PopulateObjc3SemaDispatchRuntimeArcValidationFlowArcDiagnosticsScopeReadiness(
      record, surface, deterministic_arc_diagnostics_fixit_handoff,
      deterministic_autoreleasepool_scope_handoff);
  FinalizeObjc3SemaDispatchRuntimeArcValidationFlowRecord(record);
  return record;
}
