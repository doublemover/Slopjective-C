namespace {

static Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord
BuildObjc3SemaDispatchRuntimeArcValidationFlowRecordSeed(
    const Objc3SemaPassManagerInput &input) {
  Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  return record;
}

}  // namespace
