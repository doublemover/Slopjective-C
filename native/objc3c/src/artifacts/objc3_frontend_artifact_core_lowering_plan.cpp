#include "artifacts/objc3_frontend_artifact_core_lowering_plan.h"

#include <utility>

#include "lower/contracts/control_flow_safety_lowering_contracts.h"

namespace {

using objc3::artifacts::frontend::BuildControlFlowControlFlowSafetyLoweringContract;
using objc3::artifacts::frontend::BuildDispatchAbiMarshallingContract;
using objc3::artifacts::frontend::BuildDispatchSurfaceClassificationContract;
using objc3::artifacts::frontend::BuildIdClassSelObjectPointerTypecheckContract;
using objc3::artifacts::frontend::BuildMessageSendSelectorLoweringContract;
using objc3::artifacts::frontend::BuildNilReceiverSemanticsFoldabilityContract;
using objc3::artifacts::frontend::BuildPropertySynthesisIvarBindingContract;
using objc3::artifacts::frontend::BuildRuntimeDispatchLoweringAbiContract;
using objc3::artifacts::frontend::BuildRuntimeLinkHostLinkContract;
using objc3::artifacts::frontend::BuildSuperDispatchMethodFamilyContract;
using objc3::artifacts::frontend::BuildTypeSystemOptionalKeypathLoweringContract;
using objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure;

void AddPostPipelineFailure(
    std::vector<Objc3FrontendArtifactPostPipelineFailure> &failures,
    const char *code,
    std::string message) {
  Objc3FrontendArtifactPostPipelineFailure failure;
  failure.present = true;
  failure.code = code == nullptr ? "" : code;
  failure.message = std::move(message);
  failures.push_back(std::move(failure));
}

}  // namespace

Objc3FrontendArtifactCoreLoweringPlan
BuildObjc3FrontendArtifactCoreLoweringPlan(
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3TypeSystemTypeSemanticModelSummary
        &type_system_type_semantic_model_summary,
    const Objc3ControlFlowControlFlowSemanticModelSummary
        &control_flow_control_flow_semantic_model_summary,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api) {
  Objc3FrontendArtifactCoreLoweringPlan plan;
  plan.property_synthesis_ivar_binding_contract =
      BuildPropertySynthesisIvarBindingContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3PropertySynthesisIvarBindingContract(
          plan.property_synthesis_ivar_binding_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid property synthesis/ivar binding "
        "lowering contract");
  }
  plan.property_synthesis_ivar_binding_replay_key =
      Objc3PropertySynthesisIvarBindingReplayKey(
          plan.property_synthesis_ivar_binding_contract);
  plan.property_synthesis_ivar_binding_snapshot =
      objc3::artifacts::frontend::BuildPropertySynthesisIvarBindingSnapshot(
          plan.property_synthesis_ivar_binding_contract);
  plan.property_synthesis_ivar_binding_summary =
      pipeline_result.sema_parity_surface.property_synthesis_ivar_binding_summary;
  plan.property_synthesis_ivar_binding_handoff_deterministic =
      plan.property_synthesis_ivar_binding_summary.deterministic &&
      pipeline_result.sema_parity_surface
          .deterministic_property_synthesis_ivar_binding_handoff;

  plan.id_class_sel_object_pointer_typecheck_contract =
      BuildIdClassSelObjectPointerTypecheckContract(program);
  if (!IsValidObjc3IdClassSelObjectPointerTypecheckContract(
          plan.id_class_sel_object_pointer_typecheck_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid id/Class/SEL/object-pointer "
        "typecheck lowering contract");
  }
  plan.id_class_sel_object_pointer_typecheck_replay_key =
      Objc3IdClassSelObjectPointerTypecheckReplayKey(
          plan.id_class_sel_object_pointer_typecheck_contract);
  plan.id_class_sel_object_pointer_typecheck_snapshot =
      objc3::artifacts::frontend::
          BuildIdClassSelObjectPointerTypecheckSnapshot(
              plan.id_class_sel_object_pointer_typecheck_contract);

  plan.dispatch_surface_classification_contract =
      BuildDispatchSurfaceClassificationContract(program);
  if (!IsValidObjc3DispatchSurfaceClassificationContract(
          plan.dispatch_surface_classification_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid dispatch-surface classification "
        "contract");
  }
  plan.dispatch_surface_classification_replay_key =
      Objc3DispatchSurfaceClassificationReplayKey(
          plan.dispatch_surface_classification_contract);
  plan.dispatch_surface_classification_snapshot =
      objc3::artifacts::frontend::BuildDispatchSurfaceClassificationSnapshot(
          plan.dispatch_surface_classification_contract);

  plan.message_send_selector_lowering_contract =
      BuildMessageSendSelectorLoweringContract(program);
  if (!IsValidObjc3MessageSendSelectorLoweringContract(
          plan.message_send_selector_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid message-send selector lowering "
        "contract");
  }
  plan.message_send_selector_lowering_replay_key =
      Objc3MessageSendSelectorLoweringReplayKey(
          plan.message_send_selector_lowering_contract);
  plan.message_send_selector_lowering_snapshot =
      objc3::artifacts::frontend::BuildMessageSendSelectorLoweringSnapshot(
          plan.message_send_selector_lowering_contract);

  plan.dispatch_abi_marshalling_contract =
      BuildDispatchAbiMarshallingContract(
          program, options.lowering.max_message_send_args);
  if (!IsValidObjc3DispatchAbiMarshallingContract(
          plan.dispatch_abi_marshalling_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid dispatch ABI marshalling contract");
  }
  plan.dispatch_abi_marshalling_replay_key =
      Objc3DispatchAbiMarshallingReplayKey(plan.dispatch_abi_marshalling_contract);
  plan.dispatch_abi_marshalling_snapshot =
      objc3::artifacts::frontend::BuildDispatchAbiMarshallingSnapshot(
          plan.dispatch_abi_marshalling_contract);

  plan.nil_receiver_semantics_foldability_contract =
      BuildNilReceiverSemanticsFoldabilityContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NilReceiverSemanticsFoldabilityContract(
          plan.nil_receiver_semantics_foldability_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid nil-receiver semantics/foldability "
        "contract");
  }
  plan.nil_receiver_semantics_foldability_replay_key =
      Objc3NilReceiverSemanticsFoldabilityReplayKey(
          plan.nil_receiver_semantics_foldability_contract);
  plan.nil_receiver_semantics_foldability_snapshot =
      objc3::artifacts::frontend::
          BuildNilReceiverSemanticsFoldabilitySnapshot(
              plan.nil_receiver_semantics_foldability_contract);

  plan.type_system_optional_keypath_lowering_contract =
      BuildTypeSystemOptionalKeypathLoweringContract(
          type_system_type_semantic_model_summary);
  if (!IsValidObjc3TypeSystemOptionalKeypathLoweringContract(
          plan.type_system_optional_keypath_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid Part 3 optional/key-path lowering "
        "contract");
  }
  plan.type_system_optional_keypath_lowering_replay_key =
      Objc3TypeSystemOptionalKeypathLoweringReplayKey(
          plan.type_system_optional_keypath_lowering_contract);

  plan.control_flow_control_flow_safety_lowering_contract =
      BuildControlFlowControlFlowSafetyLoweringContract(
          control_flow_control_flow_semantic_model_summary);
  if (!IsValidObjc3ControlFlowControlFlowSafetyLoweringContract(
          plan.control_flow_control_flow_safety_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid Part 5 control-flow safety lowering "
        "contract");
  }
  plan.control_flow_control_flow_safety_lowering_replay_key =
      Objc3ControlFlowControlFlowSafetyLoweringReplayKey(
          plan.control_flow_control_flow_safety_lowering_contract);

  plan.super_dispatch_method_family_contract =
      BuildSuperDispatchMethodFamilyContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3SuperDispatchMethodFamilyContract(
          plan.super_dispatch_method_family_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid super-dispatch/method-family contract");
  }
  plan.super_dispatch_method_family_replay_key =
      Objc3SuperDispatchMethodFamilyReplayKey(
          plan.super_dispatch_method_family_contract);
  plan.super_dispatch_method_family_snapshot =
      objc3::artifacts::frontend::BuildSuperDispatchMethodFamilySnapshot(
          plan.super_dispatch_method_family_contract);

  plan.runtime_link_host_link_contract =
      BuildRuntimeLinkHostLinkContract(
          plan.dispatch_abi_marshalling_contract,
          plan.nil_receiver_semantics_foldability_contract,
          options);
  if (!IsValidObjc3RuntimeLinkHostLinkContract(
          plan.runtime_link_host_link_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid runtime dispatch host-link contract");
  }
  plan.runtime_link_host_link_replay_key =
      Objc3RuntimeLinkHostLinkReplayKey(plan.runtime_link_host_link_contract);
  plan.runtime_link_host_link_snapshot =
      objc3::artifacts::frontend::BuildRuntimeLinkHostLinkSnapshot(
          plan.runtime_link_host_link_contract);

  plan.runtime_dispatch_lowering_abi_contract =
      BuildRuntimeDispatchLoweringAbiContract(
          plan.dispatch_abi_marshalling_contract,
          plan.runtime_link_host_link_contract,
          runtime_bootstrap_api);
  if (!IsValidObjc3RuntimeDispatchLoweringAbiContract(
          plan.runtime_dispatch_lowering_abi_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid runtime dispatch lowering ABI "
        "contract");
  }
  plan.runtime_dispatch_lowering_abi_replay_key =
      Objc3RuntimeDispatchLoweringAbiReplayKey(
          plan.runtime_dispatch_lowering_abi_contract);
  plan.runtime_dispatch_lowering_abi_snapshot =
      objc3::artifacts::frontend::BuildRuntimeDispatchLoweringAbiSnapshot(
          plan.runtime_dispatch_lowering_abi_contract);

  return plan;
}
