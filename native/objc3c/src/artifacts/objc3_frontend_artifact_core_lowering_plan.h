#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_control_flow_semantic_artifacts.h"
#include "artifacts/objc3_frontend_dispatch_semantic_artifacts.h"
#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactCoreLoweringPlan {
  Objc3PropertySynthesisIvarBindingContract
      property_synthesis_ivar_binding_contract;
  std::string property_synthesis_ivar_binding_replay_key;
  Objc3PropertySynthesisIvarBindingSummary
      property_synthesis_ivar_binding_summary;
  bool property_synthesis_ivar_binding_handoff_deterministic = false;
  Objc3IdClassSelObjectPointerTypecheckContract
      id_class_sel_object_pointer_typecheck_contract;
  std::string id_class_sel_object_pointer_typecheck_replay_key;
  Objc3DispatchSurfaceClassificationContract
      dispatch_surface_classification_contract;
  std::string dispatch_surface_classification_replay_key;
  Objc3MessageSendSelectorLoweringContract message_send_selector_lowering_contract;
  std::string message_send_selector_lowering_replay_key;
  Objc3DispatchAbiMarshallingContract dispatch_abi_marshalling_contract;
  std::string dispatch_abi_marshalling_replay_key;
  Objc3NilReceiverSemanticsFoldabilityContract
      nil_receiver_semantics_foldability_contract;
  std::string nil_receiver_semantics_foldability_replay_key;
  Objc3TypeSystemOptionalKeypathLoweringContract
      type_system_optional_keypath_lowering_contract;
  std::string type_system_optional_keypath_lowering_replay_key;
  Objc3ControlFlowControlFlowSafetyLoweringContract
      control_flow_control_flow_safety_lowering_contract;
  std::string control_flow_control_flow_safety_lowering_replay_key;
  Objc3SuperDispatchMethodFamilyContract super_dispatch_method_family_contract;
  std::string super_dispatch_method_family_replay_key;
  Objc3RuntimeLinkHostLinkContract runtime_link_host_link_contract;
  std::string runtime_link_host_link_replay_key;
  Objc3RuntimeDispatchLoweringAbiContract runtime_dispatch_lowering_abi_contract;
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
