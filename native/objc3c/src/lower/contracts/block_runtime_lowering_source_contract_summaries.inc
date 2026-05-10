std::string Objc3ExecutableBlockSourceClosureSummary() {
  std::ostringstream out;
  // executable-block-source-closure freeze anchor: this summary is
  // intentionally truthful about the current boundary. Parser/AST/source
  // replay for block literals is live, while runnable lowering remains a
  // fail-closed non-goal until later work.
  out << "contract=" << Expr::kObjc3ExecutableBlockSourceClosureContractId
      << ";source_model=" << Expr::kObjc3ExecutableBlockSourceSurfaceModel
      << ";evidence_model=" << Expr::kObjc3ExecutableBlockSourceEvidenceModel
      << ";non_goal_model=" << Expr::kObjc3ExecutableBlockSourceNonGoalModel
      << ";fail_closed_model=" << Expr::kObjc3ExecutableBlockSourceFailureModel
      << ";capture_lane_contract=" << kObjc3BlockLiteralCaptureLoweringLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockSourceModelCompletionSummary() {
  std::ostringstream out;
  // block-source-model-completion anchor: lane-A now upgrades the
  // frozen block source closure into a deterministic parameter/capture/invoke
  // source model that source-only frontend runs may publish before runnable
  // lowering still fails closed on native emit paths.
  out << "contract="
      << Expr::kObjc3ExecutableBlockSourceModelCompletionContractId
      << ";signature_model=" << Expr::kObjc3ExecutableBlockSignatureModel
      << ";capture_inventory_model="
      << Expr::kObjc3ExecutableBlockCaptureInventoryModel
      << ";invoke_surface_model="
      << Expr::kObjc3ExecutableBlockInvokeSurfaceModel
      << ";evidence_model="
      << Expr::kObjc3ExecutableBlockSourceModelEvidenceModel
      << ";fail_closed_model="
      << Expr::kObjc3ExecutableBlockSourceModelFailureModel
      << ";lane_contract=" << kObjc3BlockSourceModelCompletionLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockSourceStorageAnnotationSummary() {
  std::ostringstream out;
  // block-source-storage-annotation anchor: lane-A now publishes a
  // truthful byref/helper/escape-shape source inventory without claiming that
  // runnable block lowering, helper emission, or heap promotion already exist.
  out << "contract="
      << Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId
      << ";byref_storage_model=" << Expr::kObjc3ExecutableBlockByrefStorageModel
      << ";helper_intent_model="
      << Expr::kObjc3ExecutableBlockHelperIntentModel
      << ";escape_shape_model="
      << Expr::kObjc3ExecutableBlockEscapeShapeModel
      << ";lane_contract="
      << kObjc3BlockSourceStorageAnnotationLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockRuntimeSemanticRulesSummary() {
  std::ostringstream out;
  // block-runtime-semantic-rules freeze anchor: lane-B now freezes
  // the current semantic split where source-only block admission is truthful,
  // deterministic capture/byref/helper/escape annotations exist, and native
  // emit paths still fail closed before runnable block semantics land.
  out << "contract="
      << Expr::kObjc3ExecutableBlockRuntimeSemanticRulesContractId
      << ";capture_legality_model="
      << Expr::kObjc3ExecutableBlockRuntimeCaptureLegalityModel
      << ";storage_class_model="
      << Expr::kObjc3ExecutableBlockRuntimeStorageClassModel
      << ";escape_behavior_model="
      << Expr::kObjc3ExecutableBlockRuntimeEscapeBehaviorModel
      << ";helper_generation_model="
      << Expr::kObjc3ExecutableBlockRuntimeHelperGenerationModel
      << ";invocation_model="
      << Expr::kObjc3ExecutableBlockRuntimeInvocationModel
      << ";fail_closed_model="
      << Expr::kObjc3ExecutableBlockRuntimeFailClosedModel
      << ";lane_contract=" << kObjc3BlockRuntimeSemanticRulesLaneContract;
  return out.str();
}
