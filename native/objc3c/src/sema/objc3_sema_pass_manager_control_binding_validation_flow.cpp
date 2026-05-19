#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaControlBindingParityValidationReadinessRecord
BuildObjc3SemaControlBindingParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_unwind_cleanup_handoff,
    bool deterministic_async_continuation_handoff,
    bool deterministic_symbol_graph_scope_resolution_handoff,
    bool deterministic_method_lookup_override_conflict_handoff,
    bool deterministic_property_synthesis_ivar_binding_handoff,
    bool deterministic_id_class_sel_object_pointer_type_checking_handoff) {
  Objc3SemaControlBindingParityValidationReadinessRecord record;

#include "sema/objc3_sema_pass_manager_control_binding_validation_record_seed.inc"
#include "sema/objc3_sema_pass_manager_control_binding_validation_control_flow_readiness.inc"
#include "sema/objc3_sema_pass_manager_control_binding_validation_symbol_binding_readiness.inc"
#include "sema/objc3_sema_pass_manager_control_binding_validation_validation_publication.inc"

  return record;
}
