#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_artifact_dispatch_contract_snapshots.h"
#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"
#include "artifacts/objc3_frontend_dispatch_semantic_artifacts.h"
#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"
#include "lower/contracts/control_flow_safety_lowering_contracts.h"
#include "pipeline/objc3_frontend_types.h"
#include "sema/objc3_sema_contract_effects_flow_control_flow.h"

struct Objc3FrontendArtifactCoreLoweringPlan {
  Objc3PropertySynthesisIvarBindingContract
      property_synthesis_ivar_binding_contract;
  objc3::artifacts::frontend::Objc3PropertySynthesisIvarBindingSnapshot
      property_synthesis_ivar_binding_snapshot;
  std::string property_synthesis_ivar_binding_replay_key;
  Objc3PropertySynthesisIvarBindingSummary
      property_synthesis_ivar_binding_summary;
  bool property_synthesis_ivar_binding_handoff_deterministic = false;
  Objc3IdClassSelObjectPointerTypecheckContract
      id_class_sel_object_pointer_typecheck_contract;
  objc3::artifacts::frontend::Objc3IdClassSelObjectPointerTypecheckSnapshot
      id_class_sel_object_pointer_typecheck_snapshot;
  std::string id_class_sel_object_pointer_typecheck_replay_key;
  Objc3DispatchSurfaceClassificationContract
      dispatch_surface_classification_contract;
  objc3::artifacts::frontend::Objc3DispatchSurfaceClassificationSnapshot
      dispatch_surface_classification_snapshot;
  std::string dispatch_surface_classification_replay_key;
  Objc3MessageSendSelectorLoweringContract message_send_selector_lowering_contract;
  objc3::artifacts::frontend::Objc3MessageSendSelectorLoweringSnapshot
      message_send_selector_lowering_snapshot;
  std::string message_send_selector_lowering_replay_key;
  Objc3DispatchAbiMarshallingContract dispatch_abi_marshalling_contract;
  objc3::artifacts::frontend::Objc3DispatchAbiMarshallingSnapshot
      dispatch_abi_marshalling_snapshot;
  std::string dispatch_abi_marshalling_replay_key;
  Objc3NilReceiverSemanticsFoldabilityContract
      nil_receiver_semantics_foldability_contract;
  objc3::artifacts::frontend::Objc3NilReceiverSemanticsFoldabilitySnapshot
      nil_receiver_semantics_foldability_snapshot;
  std::string nil_receiver_semantics_foldability_replay_key;
  Objc3TypeSystemOptionalKeypathLoweringContract
      type_system_optional_keypath_lowering_contract;
  std::string type_system_optional_keypath_lowering_replay_key;
  Objc3ControlFlowControlFlowSafetyLoweringContract
      control_flow_control_flow_safety_lowering_contract;
  std::string control_flow_control_flow_safety_lowering_replay_key;
  Objc3SuperDispatchMethodFamilyContract super_dispatch_method_family_contract;
  objc3::artifacts::frontend::Objc3SuperDispatchMethodFamilySnapshot
      super_dispatch_method_family_snapshot;
  std::string super_dispatch_method_family_replay_key;
  Objc3RuntimeLinkHostLinkContract runtime_link_host_link_contract;
  objc3::artifacts::frontend::Objc3RuntimeLinkHostLinkSnapshot
      runtime_link_host_link_snapshot;
  std::string runtime_link_host_link_replay_key;
  Objc3RuntimeDispatchLoweringAbiContract runtime_dispatch_lowering_abi_contract;
  objc3::artifacts::frontend::Objc3RuntimeDispatchLoweringAbiSnapshot
      runtime_dispatch_lowering_abi_snapshot;
  std::string runtime_dispatch_lowering_abi_replay_key;
  std::vector<
      objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure>
      post_pipeline_failures;
};

Objc3FrontendArtifactCoreLoweringPlan
BuildObjc3FrontendArtifactCoreLoweringPlan(
    const Objc3Program &program,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3TypeSystemTypeSemanticModelSummary
        &type_system_type_semantic_model_summary,
    const Objc3ControlFlowControlFlowSemanticModelSummary
        &control_flow_control_flow_semantic_model_summary,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api);
