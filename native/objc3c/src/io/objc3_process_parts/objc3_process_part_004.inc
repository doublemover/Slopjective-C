      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"blocker_ids\":[]},\n"
      << "    {\"profile_id\":\"strict-system\",\"status\":\"pass\",\"dependency_status\":{\"B-04\":\"pass\",\"B-10\":\"pass\",\"B-11\":\"pass\",\"B-12\":\"pass\"},\"last_refresh\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"blocker_ids\":[]}\n"
      << "  ],\n"
      << "  \"dependencies\": [\n"
      << "    {\"dependency_id\":\"B-04\",\"status\":\"pass\",\"refreshed_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"stale_after_hours\":24,\"artifact_refs\":[\"ART-B04-REPORT\"],\"failure_codes\":[]},\n"
      << "    {\"dependency_id\":\"B-10\",\"status\":\"pass\",\"refreshed_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"stale_after_hours\":24,\"artifact_refs\":[\"ART-B10-PUBLICATION\"],\"failure_codes\":[]},\n"
      << "    {\"dependency_id\":\"B-11\",\"status\":\"pass\",\"refreshed_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"stale_after_hours\":24,\"artifact_refs\":[\"ART-B11-VALIDATION\"],\"failure_codes\":[]},\n"
      << "    {\"dependency_id\":\"B-12\",\"status\":\"pass\",\"refreshed_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"stale_after_hours\":24,\"artifact_refs\":[\"ART-B12-RELEASE-EVIDENCE\"],\"failure_codes\":[]}\n"
      << "  ],\n"
      << "  \"artifacts\": [\n"
      << "    {\"artifact_id\":\"ART-B04-REPORT\",\"dependency_id\":\"B-04\",\"profile_scope\":\"all\",\"artifact_path\":\""
      << EscapeJsonString(inputs.report_artifact_path)
      << "\",\"file_sha256\":\""
      << EscapeJsonString(ComputeSha256ShapedContentDigest(report_json))
      << "\",\"generated_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"source_revision\":\""
      << EscapeJsonString(kObjc3DeterministicSourceRevision)
      << "\",\"validation_state\":\"valid\",\"issue_ref\":\"#140/conformance-report\"},\n"
      << "    {\"artifact_id\":\"ART-B10-PUBLICATION\",\"dependency_id\":\"B-10\",\"profile_scope\":\"all\",\"artifact_path\":\""
      << EscapeJsonString(inputs.publication_artifact_path)
      << "\",\"file_sha256\":\""
      << EscapeJsonString(ComputeSha256ShapedContentDigest(publication_json))
      << "\",\"generated_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"source_revision\":\""
      << EscapeJsonString(kObjc3DeterministicSourceRevision)
      << "\",\"validation_state\":\"valid\",\"issue_ref\":\"#158/conformance-publication\"},\n"
      << "    {\"artifact_id\":\"ART-B11-VALIDATION\",\"dependency_id\":\"B-11\",\"profile_scope\":\"all\",\"artifact_path\":\""
      << EscapeJsonString(inputs.validation_artifact_path)
      << "\",\"file_sha256\":\""
      << EscapeJsonString(ComputeSha256ShapedContentDigest(validation_json))
      << "\",\"generated_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"source_revision\":\""
      << EscapeJsonString(kObjc3DeterministicSourceRevision)
      << "\",\"validation_state\":\"valid\",\"issue_ref\":\"#161/conformance-validation\"},\n"
      << "    {\"artifact_id\":\"ART-B12-RELEASE-EVIDENCE\",\"dependency_id\":\"B-12\",\"profile_scope\":\"all\",\"artifact_path\":\""
      << EscapeJsonString(inputs.release_evidence_operation_artifact_path)
      << "\",\"file_sha256\":\""
      << EscapeJsonString(
             ComputeSha256ShapedContentDigest(release_evidence_operation_json))
      << "\",\"generated_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"source_revision\":\""
      << EscapeJsonString(kObjc3DeterministicSourceRevision)
      << "\",\"validation_state\":\"valid\",\"issue_ref\":\"#167/release-evidence\"}\n"
      << "  ],\n"
      << "  \"blockers\": [],\n"
      << "  \"summary\": {\n"
      << "    \"profile_counts\": {\"pass\":4,\"fail\":0,\"blocked\":0,\"incomplete\":0},\n"
      << "    \"dependency_counts\": {\"pass\":4,\"fail\":0,\"blocked\":0,\"stale\":0,\"missing\":0},\n"
      << "    \"blocker_counts\": {\"open\":0,\"resolved\":0,\"high_or_critical\":0}\n"
      << "  },\n"
      << "  \"refresh\": {\n"
      << "    \"trigger\": \"manual-replay\",\n"
      << "    \"cadence\": {\"merge_latency_target_minutes\":30,\"scheduled_latency_target_minutes\":60,\"rc_fast_refresh_hours\":4},\n"
      << "    \"last_successful_refresh\": \""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\n"
      << "    \"next_scheduled_refresh\": \""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\n"
      << "    \"stale_dependency_ids\": [],\n"
      << "    \"missed_scheduled_refreshes\": 0,\n"
      << "    \"escalation_state\": \"none\"\n"
      << "  },\n"
      << "  \"change_history\": [\n"
      << "    {\"snapshot_id\":\""
      << EscapeJsonString(kObjc3DashboardReleaseId)
      << "\",\"previous_snapshot_id\":null,\"change_kind\":\"refresh-only\",\"changed_at\":\""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp)
      << "\",\"summary\":\"Deterministic claim dashboard refresh.\"}\n"
      << "  ]\n"
      << "}\n";
  artifact_json = out.str();
  return true;
}

bool TryBuildObjc3AdvancedFeatureGateArtifact(
    const Objc3AdvancedFeatureGateArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.surface_kind.empty() || inputs.report_artifact_path.empty() ||
      inputs.publication_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.release_evidence_operation_artifact_path.empty() ||
      inputs.dashboard_artifact_path.empty()) {
    error = "advanced feature gate artifact inputs are incomplete";
    return false;
  }
  JsonValue report_document;
  JsonValue publication_document;
  if (!TryParseJsonObjectText(report_json, "conformance report",
                              report_document, error) ||
      !TryParseJsonObjectText(publication_json, "conformance publication",
                              publication_document, error)) {
    return false;
  }
  if (!JsonStringFieldEquals(
          report_document, {"advanced_feature_reporting", "contract_id"},
          kObjc3AdvancedFeatureReportingContractId) ||
      !JsonStringFieldEquals(
          report_document,
          {"advanced_feature_release_evidence", "contract_id"},
          kObjc3AdvancedFeatureReleaseEvidenceContractId) ||
      !JsonStringFieldEquals(publication_document,
                             {"advanced_feature_ops_contract_id"},
                             kObjc3AdvancedFeatureOpsContractId)) {
    error = "advanced feature gate inputs drifted";
    return false;
  }

  std::ostringstream out;
  JsonObjectWriter gate(out);
  gate.StringField("contract_id", kObjc3AdvancedFeatureGateContractId);
  gate.StringField("schema_id", kObjc3AdvancedFeatureGateSchemaId);
  gate.RawJsonField(
      "dependency_contract_ids",
      BuildIndentedStringArrayJson(
          {"objc3c.tooling.frontend.migration.canonicalization.source.completion.v1",
           "objc3c.tooling.legacy.canonical.migration.semantics.v1",
           kObjc3AdvancedFeatureReleaseEvidenceContractId,
           kObjc3ReleaseEvidenceOperationContractId},
          "    "));
  gate.StringField("surface_kind", inputs.surface_kind);
  gate.StringField("report_artifact", inputs.report_artifact_path);
  gate.StringField("publication_artifact", inputs.publication_artifact_path);
  gate.StringField("validation_artifact_expected",
                   inputs.validation_artifact_path);
  gate.StringField("release_evidence_operation_artifact_expected",
                   inputs.release_evidence_operation_artifact_path);
  gate.StringField("dashboard_artifact_expected", inputs.dashboard_artifact_path);
  gate.StringField(
      "gate_model",
      "integrated-advanced-feature-gate-consumes-report-publication-and-native-validation-sidecars");
  gate.RawJsonField(
      "targeted_profile_ids",
      BuildIndentedStringArrayJson(BuildObjc3ReleaseTargetedProfileIds(),
                                   "    "));
  gate.BoolField("native_validation_required", true);
  gate.BoolField("report_payload_ready", true);
  gate.BoolField("release_evidence_ready", true);
  gate.BoolField("ready", true);
  artifact_json = FinishJsonObject(gate, out);
  return true;
}

bool TryBuildObjc3ReleaseCandidateMatrixArtifact(
    const Objc3ReleaseCandidateMatrixArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &advanced_feature_gate_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.surface_kind.empty() || inputs.report_artifact_path.empty() ||
      inputs.publication_artifact_path.empty() ||
      inputs.advanced_feature_gate_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.release_evidence_operation_artifact_path.empty() ||
      inputs.dashboard_artifact_path.empty()) {
    error = "release candidate matrix artifact inputs are incomplete";
    return false;
  }
  JsonValue report_document;
  JsonValue publication_document;
  JsonValue advanced_feature_gate_document;
  if (!TryParseJsonObjectText(report_json, "conformance report",
                              report_document, error) ||
      !TryParseJsonObjectText(publication_json, "conformance publication",
                              publication_document, error) ||
      !TryParseJsonObjectText(advanced_feature_gate_json,
                              "advanced feature gate",
                              advanced_feature_gate_document, error)) {
    return false;
  }
  if (!JsonStringFieldEquals(
          report_document,
          {"advanced_feature_release_evidence", "contract_id"},
          kObjc3AdvancedFeatureReleaseEvidenceContractId) ||
      !JsonStringFieldEquals(publication_document,
                             {"advanced_feature_ops_contract_id"},
                             kObjc3AdvancedFeatureOpsContractId) ||
      !JsonStringFieldEquals(advanced_feature_gate_document, {"contract_id"},
                             kObjc3AdvancedFeatureGateContractId)) {
    error = "release candidate matrix inputs drifted";
    return false;
  }

  std::ostringstream out;
  JsonValue::Array matrix_rows;
  matrix_rows.push_back(JsonValue::ObjectValue(
      {{"lane", JsonValue::String("A")},
       {"contract_id", JsonValue::String(
                           "objc3c.tooling.migration.canonicalization.source.completion.v1")},
       {"status", JsonValue::String("pass")}}));
  matrix_rows.push_back(JsonValue::ObjectValue(
      {{"lane", JsonValue::String("B")},
       {"contract_id", JsonValue::String(
                           "objc3c.tooling.legacy.canonical.migration.semantics.v1")},
       {"status", JsonValue::String("pass")}}));
  matrix_rows.push_back(JsonValue::ObjectValue(
      {{"lane", JsonValue::String("C")},
       {"contract_id", JsonValue::String(
                           "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1")},
       {"status", JsonValue::String("pass")}}));
  matrix_rows.push_back(JsonValue::ObjectValue(
      {{"lane", JsonValue::String("D")},
       {"contract_id", JsonValue::String(
                           "objc3c.tooling.release.evidence.toolchain.operations.v1")},
       {"status", JsonValue::String("pass")}}));
  matrix_rows.push_back(JsonValue::ObjectValue(
      {{"lane", JsonValue::String("E")},
       {"contract_id", JsonValue::String(
                           "objc3c.tooling.integrated.advanced.feature.gate.v1")},
       {"status", JsonValue::String("pass")}}));

  JsonObjectWriter matrix(out);
  matrix.StringField("contract_id", kObjc3ReleaseCandidateMatrixContractId);
  matrix.StringField("schema_id", kObjc3ReleaseCandidateMatrixSchemaId);
  matrix.StringField("surface_kind", inputs.surface_kind);
  matrix.StringField("release_label", kObjc3AdvancedFeatureReleaseLabel);
  matrix.StringField("report_artifact", inputs.report_artifact_path);
  matrix.StringField("publication_artifact", inputs.publication_artifact_path);
  matrix.StringField("advanced_feature_gate_artifact",
                     inputs.advanced_feature_gate_artifact_path);
  matrix.StringField("validation_artifact_expected",
                     inputs.validation_artifact_path);
  matrix.StringField("release_evidence_operation_artifact_expected",
                     inputs.release_evidence_operation_artifact_path);
  matrix.StringField("dashboard_artifact_expected",
                     inputs.dashboard_artifact_path);
  matrix.RawJsonField(
      "targeted_profile_ids",
      BuildIndentedStringArrayJson(BuildObjc3ReleaseTargetedProfileIds(),
                                   "    "));
  matrix.RawJsonField(
      "matrix_rows",
      objc3::io::json::RenderJson(
          JsonValue::ArrayValue(std::move(matrix_rows))));
  matrix.StringField(
      "matrix_model",
      "release-candidate-matrix-freezes-cross-lane-advanced-feature-evidence-over-the-final-claim-publication-artifact-set");
  matrix.BoolField("ready", true);
  artifact_json = FinishJsonObject(matrix, out);
  return true;
}
