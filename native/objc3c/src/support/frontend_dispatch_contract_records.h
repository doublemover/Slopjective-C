#pragma once

#include <cstddef>
#include <string>

namespace objc3::pipeline::frontend {

inline constexpr std::size_t kDispatchRuntimeDefaultArgs = 4;
inline constexpr const char *kDispatchRuntimeSymbol =
    "objc3_runtime_dispatch_i32";
inline constexpr const char *kDispatchSurfaceLiveRuntimeEntrypointFamily =
    "objc3_runtime_dispatch_i32-canonical-live-runtime";
inline constexpr const char *kDispatchSurfaceDirectDispatchBinding =
    "reserved-non-goal";
inline constexpr const char *kRuntimeDispatchLoweringAbiBoundaryModel =
    "canonical-runtime-dispatch-default-target";
inline constexpr const char *kRuntimeDispatchLoweringCanonicalEntrypointSymbol =
    "objc3_runtime_dispatch_i32";
inline constexpr const char *kRuntimeDispatchLoweringSelectorLookupSymbol =
    "objc3_runtime_lookup_selector";
inline constexpr const char *kRuntimeDispatchLoweringSelectorHandleType =
    "objc3_runtime_selector_handle";
inline constexpr const char *kRuntimeDispatchLoweringReceiverAbiType = "i32";
inline constexpr const char *kRuntimeDispatchLoweringSelectorAbiType = "ptr";
inline constexpr const char *kRuntimeDispatchLoweringArgumentAbiType = "i32";
inline constexpr const char *kRuntimeDispatchLoweringResultAbiType = "i32";
inline constexpr const char *kRuntimeDispatchLoweringSelectorOperandModel =
    "selector-cstring-pointer-remains-lowered-operand-until-next-runtime-phase";
inline constexpr const char *kRuntimeDispatchLoweringSelectorHandleModel =
    "runtime-lookup-produces-selector-handle-before-live-dispatch";
inline constexpr const char *kRuntimeDispatchLoweringArgumentPaddingModel =
    "zero-pad-to-fixed-runtime-arg-slot-count";
inline constexpr const char *kRuntimeDispatchLoweringDefaultTargetModel =
    "default-lowering-target-is-canonical-runtime-entrypoint";
inline constexpr const char *kRuntimeDispatchLoweringStrictDispatchErrorModel =
    "resolved-runtime-call-or-hard-diagnostic-error-only";
inline constexpr const char *kRuntimeDispatchLoweringDeferredCasesModel =
    "direct-dispatch-remains-fail-closed-after-live-cutover";

}  // namespace objc3::pipeline::frontend

struct Objc3MethodLookupOverrideConflictContract {
  std::size_t method_lookup_sites = 0;
  std::size_t method_lookup_hits = 0;
  std::size_t method_lookup_misses = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  bool deterministic = true;
};

struct Objc3PropertySynthesisIvarBindingContract {
  std::size_t property_synthesis_sites = 0;
  std::size_t property_synthesis_explicit_ivar_bindings = 0;
  std::size_t property_synthesis_default_ivar_bindings = 0;
  std::size_t interface_owned_property_synthesis_sites = 0;
  std::size_t implementation_property_redeclaration_sites = 0;
  std::size_t ivar_binding_sites = 0;
  std::size_t ivar_binding_resolved = 0;
  std::size_t ivar_binding_missing = 0;
  std::size_t ivar_binding_conflicts = 0;
  bool deterministic = true;
};

struct Objc3IdClassSelObjectPointerTypecheckContract {
  std::size_t id_typecheck_sites = 0;
  std::size_t class_typecheck_sites = 0;
  std::size_t sel_typecheck_sites = 0;
  std::size_t object_pointer_typecheck_sites = 0;
  std::size_t total_typecheck_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchDispatchControlLoweringContract {
  std::size_t direct_call_candidate_sites = 0;
  std::size_t direct_members_defaulted_sites = 0;
  std::size_t dynamic_opt_out_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t override_legality_sites = 0;
  std::size_t metadata_preserved_callable_sites = 0;
  std::size_t metadata_preserved_container_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchSurfaceClassificationContract {
  std::size_t instance_dispatch_sites = 0;
  std::size_t class_dispatch_sites = 0;
  std::size_t super_dispatch_sites = 0;
  std::size_t direct_dispatch_sites = 0;
  std::size_t dynamic_dispatch_sites = 0;
  std::string instance_entrypoint_family =
      objc3::pipeline::frontend::kDispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string class_entrypoint_family =
      objc3::pipeline::frontend::kDispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string super_entrypoint_family =
      objc3::pipeline::frontend::kDispatchSurfaceLiveRuntimeEntrypointFamily;
  std::string direct_entrypoint_family =
      objc3::pipeline::frontend::kDispatchSurfaceDirectDispatchBinding;
  std::string dynamic_entrypoint_family =
      objc3::pipeline::frontend::kDispatchSurfaceLiveRuntimeEntrypointFamily;
  bool deterministic = true;
};

struct Objc3MessageSendSelectorLoweringContract {
  std::size_t message_send_sites = 0;
  std::size_t unary_selector_sites = 0;
  std::size_t keyword_selector_sites = 0;
  std::size_t selector_piece_sites = 0;
  std::size_t argument_expression_sites = 0;
  std::size_t receiver_expression_sites = 0;
  std::size_t selector_literal_entries = 0;
  std::size_t selector_literal_characters = 0;
  bool deterministic = true;
};

struct Objc3DispatchAbiMarshallingContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_slots_marshaled = 0;
  std::size_t selector_slots_marshaled = 0;
  std::size_t argument_value_slots_marshaled = 0;
  std::size_t argument_padding_slots_marshaled = 0;
  std::size_t argument_total_slots_marshaled = 0;
  std::size_t total_marshaled_slots = 0;
  std::size_t runtime_dispatch_arg_slots =
      objc3::pipeline::frontend::kDispatchRuntimeDefaultArgs;
  bool deterministic = true;
};

struct Objc3NilReceiverSemanticsFoldabilityContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_nil_literal_sites = 0;
  std::size_t nil_receiver_semantics_enabled_sites = 0;
  std::size_t nil_receiver_foldable_sites = 0;
  std::size_t nil_receiver_runtime_dispatch_required_sites = 0;
  std::size_t non_nil_receiver_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3SuperDispatchMethodFamilyContract {
  std::size_t message_send_sites = 0;
  std::size_t receiver_super_identifier_sites = 0;
  std::size_t super_dispatch_enabled_sites = 0;
  std::size_t super_dispatch_requires_class_context_sites = 0;
  std::size_t method_family_alloc_sites = 0;
  std::size_t method_family_init_sites = 0;
  std::size_t method_family_copy_sites = 0;
  std::size_t method_family_mutable_copy_sites = 0;
  std::size_t method_family_new_sites = 0;
  std::size_t method_family_none_sites = 0;
  std::size_t method_family_returns_retained_result_sites = 0;
  std::size_t method_family_returns_related_result_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3RuntimeLinkHostLinkContract {
  std::size_t message_send_sites = 0;
  std::size_t runtime_link_required_sites = 0;
  std::size_t runtime_link_elided_sites = 0;
  std::size_t runtime_dispatch_arg_slots =
      objc3::pipeline::frontend::kDispatchRuntimeDefaultArgs;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  std::size_t contract_violation_sites = 0;
  std::string runtime_dispatch_symbol =
      objc3::pipeline::frontend::kDispatchRuntimeSymbol;
  bool default_runtime_dispatch_symbol_binding = true;
  bool deterministic = true;
};

struct Objc3RuntimeDispatchLoweringAbiContract {
  std::size_t message_send_sites = 0;
  std::size_t fixed_argument_slot_count =
      objc3::pipeline::frontend::kDispatchRuntimeDefaultArgs;
  std::size_t runtime_dispatch_parameter_count = 0;
  std::string lowering_boundary_model =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringAbiBoundaryModel;
  std::string canonical_runtime_dispatch_symbol =
      objc3::pipeline::frontend::
          kRuntimeDispatchLoweringCanonicalEntrypointSymbol;
  std::string default_lowering_target_symbol =
      objc3::pipeline::frontend::kDispatchRuntimeSymbol;
  std::string selector_lookup_symbol =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringSelectorLookupSymbol;
  std::string selector_handle_type =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringSelectorHandleType;
  std::string receiver_abi_type =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringReceiverAbiType;
  std::string selector_abi_type =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringSelectorAbiType;
  std::string argument_abi_type =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringArgumentAbiType;
  std::string result_abi_type =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringResultAbiType;
  std::string selector_operand_model =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringSelectorOperandModel;
  std::string selector_handle_model =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringSelectorHandleModel;
  std::string argument_padding_model =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringArgumentPaddingModel;
  std::string default_lowering_target_model =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringDefaultTargetModel;
  std::string strict_dispatch_error_model =
      objc3::pipeline::frontend::
          kRuntimeDispatchLoweringStrictDispatchErrorModel;
  std::string deferred_cases_model =
      objc3::pipeline::frontend::kRuntimeDispatchLoweringDeferredCasesModel;
  bool fail_closed = true;
  bool deterministic = true;
};

bool IsValidObjc3MethodLookupOverrideConflictContract(
    const Objc3MethodLookupOverrideConflictContract &contract);
std::string Objc3MethodLookupOverrideConflictReplayKey(
    const Objc3MethodLookupOverrideConflictContract &contract);
Objc3PropertySynthesisIvarBindingContract
Objc3DefaultPropertySynthesisIvarBindingContract(
    std::size_t property_synthesis_sites,
    bool deterministic = true);
bool IsValidObjc3PropertySynthesisIvarBindingContract(
    const Objc3PropertySynthesisIvarBindingContract &contract);
std::string Objc3PropertySynthesisIvarBindingReplayKey(
    const Objc3PropertySynthesisIvarBindingContract &contract);
bool IsValidObjc3IdClassSelObjectPointerTypecheckContract(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract);
std::string Objc3IdClassSelObjectPointerTypecheckReplayKey(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract);
bool IsValidObjc3DispatchDispatchControlLoweringContract(
    const Objc3DispatchDispatchControlLoweringContract &contract);
std::string Objc3DispatchDispatchControlLoweringReplayKey(
    const Objc3DispatchDispatchControlLoweringContract &contract);
std::string Objc3DispatchDispatchMetadataInterfacePreservationSummary();
bool IsValidObjc3DispatchSurfaceClassificationContract(
    const Objc3DispatchSurfaceClassificationContract &contract);
std::string Objc3DispatchSurfaceClassificationReplayKey(
    const Objc3DispatchSurfaceClassificationContract &contract);
bool IsValidObjc3MessageSendSelectorLoweringContract(
    const Objc3MessageSendSelectorLoweringContract &contract);
std::string Objc3MessageSendSelectorLoweringReplayKey(
    const Objc3MessageSendSelectorLoweringContract &contract);
bool IsValidObjc3DispatchAbiMarshallingContract(
    const Objc3DispatchAbiMarshallingContract &contract);
std::string Objc3DispatchAbiMarshallingReplayKey(
    const Objc3DispatchAbiMarshallingContract &contract);
bool IsValidObjc3NilReceiverSemanticsFoldabilityContract(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract);
std::string Objc3NilReceiverSemanticsFoldabilityReplayKey(
    const Objc3NilReceiverSemanticsFoldabilityContract &contract);
bool IsValidObjc3SuperDispatchMethodFamilyContract(
    const Objc3SuperDispatchMethodFamilyContract &contract);
std::string Objc3SuperDispatchMethodFamilyReplayKey(
    const Objc3SuperDispatchMethodFamilyContract &contract);
bool IsValidObjc3RuntimeLinkHostLinkContract(
    const Objc3RuntimeLinkHostLinkContract &contract);
std::string Objc3RuntimeLinkHostLinkReplayKey(
    const Objc3RuntimeLinkHostLinkContract &contract);
bool IsValidObjc3RuntimeDispatchLoweringAbiContract(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
std::string Objc3RuntimeDispatchLoweringAbiReplayKey(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
std::string Objc3RuntimeDispatchLoweringAbiBoundarySummary(
    const Objc3RuntimeDispatchLoweringAbiContract &contract);
