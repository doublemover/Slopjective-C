}

std::string BuildExecutableMetadataDebugProjectionRowDescriptor(
    const Objc3ExecutableMetadataDebugProjectionMatrixRow &row) {
  std::ostringstream out;
  out << row.row_key << "|" << row.artifact_kind << "|" << row.fixture_path << "|"
      << row.emit_prefix << "|" << row.artifact_relative_path << "|"
      << row.probe_command << "|" << row.inspection_command << "|"
      << row.expected_anchor;
  return out.str();
}

std::string BuildExecutableMetadataDebugProjectionReplayKey(
    const Objc3ExecutableMetadataDebugProjectionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id << ";typed_handoff_contract_id="
      << summary.typed_lowering_handoff_contract_id
      << ";source_graph_contract_id=" << summary.source_graph_contract_id
      << ";named_metadata_name=" << summary.named_metadata_name
      << ";manifest_surface_path=" << summary.manifest_surface_path
      << ";typed_handoff_surface_path=" << summary.typed_handoff_surface_path
      << ";source_graph_surface_path=" << summary.source_graph_surface_path;
  for (std::size_t index = 0; index < summary.rows.size(); ++index) {
    out << ";row[" << index << "]="
        << BuildExecutableMetadataDebugProjectionRowDescriptor(
               summary.rows[index]);
  }
  return out.str();
}

Objc3ExecutableMetadataDebugProjectionSummary
BuildExecutableMetadataDebugProjectionSummary(
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff) {
  Objc3ExecutableMetadataDebugProjectionSummary summary;
  summary.fail_closed = true;
  summary.matrix_published = true;
  summary.manifest_debug_surface_published = true;
  summary.ir_named_metadata_published = true;
  summary.active_typed_handoff_ready =
      IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          executable_metadata_typed_lowering_handoff);
  if (summary.active_typed_handoff_ready) {
    summary.active_typed_handoff_replay_key =
        executable_metadata_typed_lowering_handoff.replay_key;
  }
  summary.rows[0] = Objc3ExecutableMetadataDebugProjectionMatrixRow{
      kObjc3ExecutableMetadataDebugProjectionClassManifestRowKey,
      "manifest",
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
      kObjc3ExecutableMetadataDebugProjectionManifestRelativePath,
      kObjc3ExecutableMetadataDebugProjectionClassProbeCommand,
      kObjc3ExecutableMetadataDebugProjectionManifestInspectionCommand,
      kObjc3ExecutableMetadataDebugProjectionManifestSurfacePath};
  summary.rows[1] = Objc3ExecutableMetadataDebugProjectionMatrixRow{
      kObjc3ExecutableMetadataDebugProjectionCategoryManifestRowKey,
      "manifest",
      kObjc3ExecutableMetadataDebugProjectionCategoryFixturePath,
      kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
      kObjc3ExecutableMetadataDebugProjectionManifestRelativePath,
      kObjc3ExecutableMetadataDebugProjectionCategoryProbeCommand,
      kObjc3ExecutableMetadataDebugProjectionManifestInspectionCommand,
      kObjc3ExecutableMetadataTypedLoweringHandoffManifestSurfacePath};
  summary.rows[2] = Objc3ExecutableMetadataDebugProjectionMatrixRow{
      kObjc3ExecutableMetadataDebugProjectionIrNamedMetadataRowKey,
      "llvm-ir",
      kObjc3ExecutableMetadataDebugProjectionIrFixturePath,
      kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
      kObjc3ExecutableMetadataDebugProjectionIrRelativePath,
      kObjc3ExecutableMetadataDebugProjectionIrProbeCommand,
      kObjc3ExecutableMetadataDebugProjectionIrInspectionCommand,
      kObjc3ExecutableMetadataDebugProjectionNamedMetadataName};
  summary.matrix_row_count = summary.rows.size();
  summary.replay_key =
      BuildExecutableMetadataDebugProjectionReplayKey(summary);
  summary.replay_anchor_deterministic = !summary.replay_key.empty();
  if (!IsReadyObjc3ExecutableMetadataDebugProjectionSummary(summary)) {
    summary.failure_reason =
        "executable metadata debug projection matrix contract is incomplete";
  }
  return summary;
}

std::string BuildExecutableMetadataDebugProjectionSummaryJson(
    const Objc3ExecutableMetadataDebugProjectionSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"typed_lowering_handoff_contract_id\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_contract_id)
      << "\",\"source_graph_contract_id\":\""
      << EscapeJsonString(summary.source_graph_contract_id)
      << "\",\"named_metadata_name\":\""
      << EscapeJsonString(summary.named_metadata_name)
      << "\",\"manifest_surface_path\":\""
      << EscapeJsonString(summary.manifest_surface_path)
      << "\",\"typed_handoff_surface_path\":\""
      << EscapeJsonString(summary.typed_handoff_surface_path)
      << "\",\"source_graph_surface_path\":\""
      << EscapeJsonString(summary.source_graph_surface_path)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataDebugProjectionSummary(summary)
              ? "true"
              : "false")
      << ",\"matrix_published\":"
      << (summary.matrix_published ? "true" : "false")
      << ",\"fail_closed\":"
      << (summary.fail_closed ? "true" : "false")
      << ",\"manifest_debug_surface_published\":"
      << (summary.manifest_debug_surface_published ? "true" : "false")
      << ",\"ir_named_metadata_published\":"
      << (summary.ir_named_metadata_published ? "true" : "false")
      << ",\"replay_anchor_deterministic\":"
      << (summary.replay_anchor_deterministic ? "true" : "false")
      << ",\"active_typed_handoff_ready\":"
      << (summary.active_typed_handoff_ready ? "true" : "false")
      << ",\"matrix_row_count\":" << summary.matrix_row_count
      << ",\"rows\":[";
  for (std::size_t index = 0; index < summary.rows.size(); ++index) {
    const auto &row = summary.rows[index];
    if (index != 0u) {
      out << ",";
    }
    out << "{\"row_key\":\"" << EscapeJsonString(row.row_key)
        << "\",\"artifact_kind\":\"" << EscapeJsonString(row.artifact_kind)
        << "\",\"fixture_path\":\"" << EscapeJsonString(row.fixture_path)
        << "\",\"emit_prefix\":\"" << EscapeJsonString(row.emit_prefix)
        << "\",\"artifact_relative_path\":\""
        << EscapeJsonString(row.artifact_relative_path)
        << "\",\"probe_command\":\"" << EscapeJsonString(row.probe_command)
        << "\",\"inspection_command\":\""
        << EscapeJsonString(row.inspection_command)
        << "\",\"expected_anchor\":\""
        << EscapeJsonString(row.expected_anchor) << "\"}";
  }
  out << "],\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"active_typed_handoff_replay_key\":\""
      << EscapeJsonString(summary.active_typed_handoff_replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataRuntimeIngestPackagingReplayKey(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";typed_contract=" << summary.typed_lowering_handoff_contract_id
      << ";debug_contract=" << summary.debug_projection_contract_id
      << ";packaging_surface_path=" << summary.packaging_surface_path
      << ";typed_handoff_surface_path=" << summary.typed_handoff_surface_path
      << ";debug_projection_surface_path="
      << summary.debug_projection_surface_path
      << ";payload_model=" << summary.packaging_payload_model
      << ";transport_artifact=" << summary.transport_artifact_relative_path
      << ";typed_replay=" << summary.typed_lowering_handoff_replay_key
      << ";debug_replay=" << summary.debug_projection_replay_key;
  return out.str();
}

Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
BuildExecutableMetadataRuntimeIngestPackagingContractSummary(
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff,
    const Objc3ExecutableMetadataDebugProjectionSummary
        &executable_metadata_debug_projection) {
  Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.manifest_transport_frozen = true;
  summary.runtime_section_emission_not_yet_landed = true;
  summary.startup_registration_not_yet_landed = true;
  summary.runtime_loader_registration_not_yet_landed = true;
  summary.explicit_non_goals_published = true;
  summary.typed_lowering_handoff_ready =
      IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          executable_metadata_typed_lowering_handoff);
  summary.debug_projection_ready =
      IsReadyObjc3ExecutableMetadataDebugProjectionSummary(
          executable_metadata_debug_projection);
  if (summary.typed_lowering_handoff_ready) {
    summary.typed_lowering_handoff_replay_key =
        executable_metadata_typed_lowering_handoff.replay_key;
  }
  if (summary.debug_projection_ready) {
    summary.debug_projection_replay_key =
        executable_metadata_debug_projection.replay_key;
  }
  summary.ready_for_packaging_implementation =
      summary.typed_lowering_handoff_ready && summary.debug_projection_ready;
  if (summary.ready_for_packaging_implementation) {
    summary.replay_key =
        BuildExecutableMetadataRuntimeIngestPackagingReplayKey(summary);
  }
  if (!IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
          summary)) {
    summary.failure_reason =
        "runtime ingest packaging contract boundary is incomplete";
  }
  return summary;
}

std::string BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"typed_lowering_handoff_contract_id\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_contract_id)
      << "\",\"debug_projection_contract_id\":\""
      << EscapeJsonString(summary.debug_projection_contract_id)
      << "\",\"packaging_surface_path\":\""
      << EscapeJsonString(summary.packaging_surface_path)
      << "\",\"typed_handoff_surface_path\":\""
      << EscapeJsonString(summary.typed_handoff_surface_path)
      << "\",\"debug_projection_surface_path\":\""
      << EscapeJsonString(summary.debug_projection_surface_path)
      << "\",\"packaging_payload_model\":\""
      << EscapeJsonString(summary.packaging_payload_model)
      << "\",\"transport_artifact_relative_path\":\""
      << EscapeJsonString(summary.transport_artifact_relative_path)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
              summary)
              ? "true"
              : "false")
      << ",\"boundary_frozen\":"
      << (summary.boundary_frozen ? "true" : "false")
      << ",\"fail_closed\":"
      << (summary.fail_closed ? "true" : "false")
      << ",\"typed_lowering_handoff_ready\":"
      << (summary.typed_lowering_handoff_ready ? "true" : "false")
      << ",\"debug_projection_ready\":"
      << (summary.debug_projection_ready ? "true" : "false")
      << ",\"manifest_transport_frozen\":"
      << (summary.manifest_transport_frozen ? "true" : "false")
      << ",\"runtime_section_emission_not_yet_landed\":"
      << (summary.runtime_section_emission_not_yet_landed ? "true" : "false")
      << ",\"startup_registration_not_yet_landed\":"
      << (summary.startup_registration_not_yet_landed ? "true" : "false")
      << ",\"runtime_loader_registration_not_yet_landed\":"
      << (summary.runtime_loader_registration_not_yet_landed ? "true"
                                                             : "false")
      << ",\"explicit_non_goals_published\":"
      << (summary.explicit_non_goals_published ? "true" : "false")
      << ",\"ready_for_packaging_implementation\":"
      << (summary.ready_for_packaging_implementation ? "true" : "false")
      << ",\"typed_lowering_handoff_replay_key\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_replay_key)
      << "\",\"debug_projection_replay_key\":\""
      << EscapeJsonString(summary.debug_projection_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

void AppendRuntimeIngestBinaryEnvelopeU32(std::string &payload,
                                          std::uint32_t value) {
  for (std::uint32_t shift = 0; shift < 32u; shift += 8u) {
    payload.push_back(
        static_cast<char>((value >> shift) & static_cast<std::uint32_t>(0xFFu)));
  }
}

void AppendRuntimeIngestBinaryEnvelopeChunk(std::string &payload,
                                            const std::string &chunk_name,
                                            const std::string &chunk_payload) {
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, static_cast<std::uint32_t>(chunk_name.size()));
  payload.append(chunk_name);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, static_cast<std::uint32_t>(chunk_payload.size()));
  payload.append(chunk_payload);
}

std::string BuildExecutableMetadataRuntimeIngestBinaryEnvelope(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &packaging_contract,
    const Objc3ExecutableMetadataTypedLoweringHandoff &typed_lowering_handoff,
    const Objc3ExecutableMetadataDebugProjectionSummary &debug_projection) {
  if (!IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
          packaging_contract) ||
      !IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          typed_lowering_handoff) ||
      !IsReadyObjc3ExecutableMetadataDebugProjectionSummary(debug_projection)) {
    return {};
  }

  const std::string packaging_json =
      BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson(
          packaging_contract);
  const std::string typed_handoff_json =
      BuildExecutableMetadataTypedLoweringHandoffJson(typed_lowering_handoff);
  const std::string debug_projection_json =
      BuildExecutableMetadataDebugProjectionSummaryJson(debug_projection);

  std::string payload;
  payload.reserve(
      std::char_traits<char>::length(
          kObjc3ExecutableMetadataRuntimeIngestBinaryMagic) +
      sizeof(std::uint32_t) * 8u + packaging_json.size() +
      typed_handoff_json.size() + debug_projection_json.size() + 256u);
  payload.append(kObjc3ExecutableMetadataRuntimeIngestBinaryMagic);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeVersion);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeChunkCount);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload,
      kObjc3ExecutableMetadataRuntimeIngestBinaryPackagingChunkName,
      packaging_json);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryTypedHandoffChunkName,
      typed_handoff_json);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryDebugProjectionChunkName,
      debug_projection_json);
  return payload;
}

std::string BuildExecutableMetadataRuntimeIngestBinaryBoundaryReplayKey(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";packaging_contract_id=" << summary.packaging_contract_id
      << ";typed_contract_id=" << summary.typed_lowering_handoff_contract_id
      << ";debug_contract_id=" << summary.debug_projection_contract_id
      << ";packaging_surface_path=" << summary.packaging_surface_path
      << ";binary_boundary_surface_path=" << summary.binary_boundary_surface_path
      << ";payload_model=" << summary.payload_model
      << ";envelope_format=" << summary.envelope_format
      << ";artifact_relative_path=" << summary.artifact_relative_path
      << ";artifact_suffix=" << summary.artifact_suffix
      << ";binary_magic=" << summary.binary_magic
      << ";envelope_version=" << summary.envelope_version
      << ";chunk_count=" << summary.chunk_count
      << ";payload_bytes=" << summary.payload_bytes
      << ";packaging_replay=" << summary.packaging_contract_replay_key
      << ";typed_replay=" << summary.typed_lowering_handoff_replay_key
      << ";debug_replay=" << summary.debug_projection_replay_key;
  return out.str();
}

Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
BuildExecutableMetadataRuntimeIngestBinaryBoundarySummary(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &packaging_contract,
    const Objc3ExecutableMetadataTypedLoweringHandoff &typed_lowering_handoff,
    const Objc3ExecutableMetadataDebugProjectionSummary &debug_projection,
    const std::string &binary_payload) {
  Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary summary;
  summary.fail_closed = true;
  summary.packaging_contract_ready =
      IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
          packaging_contract);
  summary.typed_lowering_handoff_ready =
      IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          typed_lowering_handoff);
  summary.debug_projection_ready =
      IsReadyObjc3ExecutableMetadataDebugProjectionSummary(debug_projection);
  if (summary.packaging_contract_ready) {
    summary.packaging_contract_replay_key = packaging_contract.replay_key;
  }
  if (summary.typed_lowering_handoff_ready) {
    summary.typed_lowering_handoff_replay_key = typed_lowering_handoff.replay_key;
  }
  if (summary.debug_projection_ready) {
    summary.debug_projection_replay_key = debug_projection.replay_key;
  }
  summary.binary_payload_present = !binary_payload.empty();
  summary.binary_boundary_emitted = summary.binary_payload_present;
  summary.binary_envelope_deterministic =
      summary.binary_payload_present && summary.packaging_contract_ready &&
      summary.typed_lowering_handoff_ready &&
      summary.debug_projection_ready;
  summary.ready_for_section_emission_handoff =
      summary.packaging_contract_ready && summary.binary_envelope_deterministic;
  summary.payload_bytes = binary_payload.size();
  if (summary.ready_for_section_emission_handoff) {
    summary.replay_key =
        BuildExecutableMetadataRuntimeIngestBinaryBoundaryReplayKey(summary);
  }
  if (!IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
          summary)) {
    summary.failure_reason =
        "runtime ingest binary boundary payload is incomplete";
  }
  return summary;
}

std::string BuildExecutableMetadataRuntimeIngestBinaryBoundarySummaryJson(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"packaging_contract_id\":\""
      << EscapeJsonString(summary.packaging_contract_id)
      << "\",\"typed_lowering_handoff_contract_id\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_contract_id)
      << "\",\"debug_projection_contract_id\":\""
      << EscapeJsonString(summary.debug_projection_contract_id)
      << "\",\"packaging_surface_path\":\""
      << EscapeJsonString(summary.packaging_surface_path)
      << "\",\"binary_boundary_surface_path\":\""
      << EscapeJsonString(summary.binary_boundary_surface_path)
      << "\",\"payload_model\":\""
      << EscapeJsonString(summary.payload_model)
      << "\",\"envelope_format\":\""
      << EscapeJsonString(summary.envelope_format)
      << "\",\"artifact_relative_path\":\""
      << EscapeJsonString(summary.artifact_relative_path)
      << "\",\"artifact_suffix\":\""
      << EscapeJsonString(summary.artifact_suffix)
      << "\",\"binary_magic\":\""
      << EscapeJsonString(summary.binary_magic)
      << "\",\"envelope_version\":" << summary.envelope_version
      << ",\"chunk_count\":" << summary.chunk_count
      << ",\"chunk_names\":[";
  for (std::size_t i = 0; i < summary.chunk_names.size(); ++i) {
    out << "\"" << EscapeJsonString(summary.chunk_names[i]) << "\"";
    if (i + 1u != summary.chunk_names.size()) {
      out << ",";
    }
  }
  out << "],\"ready\":"
      << (IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
              summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"packaging_contract_ready\":"
      << (summary.packaging_contract_ready ? "true" : "false")
      << ",\"typed_lowering_handoff_ready\":"
      << (summary.typed_lowering_handoff_ready ? "true" : "false")
      << ",\"debug_projection_ready\":"
      << (summary.debug_projection_ready ? "true" : "false")
      << ",\"binary_payload_present\":"
      << (summary.binary_payload_present ? "true" : "false")
      << ",\"binary_boundary_emitted\":"
      << (summary.binary_boundary_emitted ? "true" : "false")
      << ",\"binary_envelope_deterministic\":"
      << (summary.binary_envelope_deterministic ? "true" : "false")
      << ",\"ready_for_section_emission_handoff\":"
      << (summary.ready_for_section_emission_handoff ? "true" : "false")
      << ",\"payload_bytes\":" << summary.payload_bytes
      << ",\"packaging_contract_replay_key\":\""
      << EscapeJsonString(summary.packaging_contract_replay_key)
      << "\",\"typed_lowering_handoff_replay_key\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_replay_key)
      << "\",\"debug_projection_replay_key\":\""
      << EscapeJsonString(summary.debug_projection_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

struct Objc3ParserDiagnosticCodeCoverage {
  std::size_t unique_code_count = 0;
  std::uint64_t unique_code_fingerprint = 1469598103934665603ull;
  bool deterministic_surface = true;
};

std::string TryExtractDiagnosticCode(const std::string &diag_text, bool &ok) {
  ok = false;
  const std::size_t end = diag_text.size();
  if (end < 3u || diag_text[end - 1] != ']') {
    return std::string{};
  }
  const std::size_t begin = diag_text.rfind('[');
  if (begin == std::string::npos || begin + 2u >= end) {
    return std::string{};
  }
  const std::string code = diag_text.substr(begin + 1u, end - begin - 2u);
  if (code.empty()) {
    return std::string{};
  }
  ok = true;
  return code;
}

std::uint64_t MixParserDiagnosticCodeFingerprint(std::uint64_t fingerprint, const std::string &code) {
  constexpr std::uint64_t kFnvPrime = 1099511628211ull;
  fingerprint = (fingerprint ^ static_cast<std::uint64_t>(code.size())) * kFnvPrime;
  for (const unsigned char c : code) {
    fingerprint = (fingerprint ^ static_cast<std::uint64_t>(c)) * kFnvPrime;
  }
  return fingerprint;
}

Objc3ParserDiagnosticCodeCoverage BuildObjc3ParserDiagnosticCodeCoverage(
    const std::vector<std::string> &parser_diagnostics) {
  Objc3ParserDiagnosticCodeCoverage coverage;
  std::unordered_set<std::string> unique_codes;
  unique_codes.reserve(parser_diagnostics.size());
  for (const auto &diag_text : parser_diagnostics) {
    bool code_ok = false;
    const std::string code = TryExtractDiagnosticCode(diag_text, code_ok);
    if (!code_ok) {
      coverage.deterministic_surface = false;
      continue;
    }
    unique_codes.insert(code);
  }
  std::vector<std::string> sorted_codes(unique_codes.begin(), unique_codes.end());
  std::sort(sorted_codes.begin(), sorted_codes.end());
  coverage.unique_code_count = sorted_codes.size();
  for (const auto &code : sorted_codes) {
    coverage.unique_code_fingerprint =
        MixParserDiagnosticCodeFingerprint(coverage.unique_code_fingerprint, code);
  }
  return coverage;
}

Objc3PropertySynthesisIvarBindingContract BuildPropertySynthesisIvarBindingContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  const Objc3PropertySynthesisIvarBindingSummary &summary =
      sema_parity_surface.property_synthesis_ivar_binding_summary;
  Objc3PropertySynthesisIvarBindingContract contract;
  contract.property_synthesis_sites = summary.property_synthesis_sites;
  contract.property_synthesis_explicit_ivar_bindings =
      summary.property_synthesis_explicit_ivar_bindings;
  contract.property_synthesis_default_ivar_bindings =
      summary.property_synthesis_default_ivar_bindings;
  contract.interface_owned_property_synthesis_sites =
      summary.interface_owned_property_synthesis_sites;
  contract.implementation_property_redeclaration_sites =
      summary.implementation_property_redeclaration_sites;
  contract.ivar_binding_sites = summary.ivar_binding_sites;
  contract.ivar_binding_resolved = summary.ivar_binding_resolved;
  contract.ivar_binding_missing = summary.ivar_binding_missing;
  contract.ivar_binding_conflicts = summary.ivar_binding_conflicts;
  contract.deterministic =
      summary.deterministic &&
      sema_parity_surface.deterministic_property_synthesis_ivar_binding_handoff;
  return contract;
}

Objc3RuntimeMetadataSectionAbiFreezeSummary
BuildRuntimeMetadataSectionAbiFreezeSummary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  Objc3RuntimeMetadataSectionAbiFreezeSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.object_file_section_inventory_frozen = true;
  summary.symbol_policy_frozen = true;
  summary.visibility_model_frozen = true;
  summary.retention_policy_frozen = true;
  summary.runtime_metadata_source_boundary_ready =
      IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
          runtime_metadata_source_ownership);
  summary.runtime_export_legality_boundary_ready =
      IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality);
  summary.runtime_export_enforcement_ready =
      IsReadyObjc3RuntimeExportEnforcementSummary(runtime_export_enforcement);
  summary.ready_for_section_scaffold =
      summary.runtime_metadata_source_boundary_ready &&
      summary.runtime_export_legality_boundary_ready &&
      summary.runtime_export_enforcement_ready;
  if (!summary.ready_for_section_scaffold) {
    summary.failure_reason =
        "runtime metadata section ABI freeze prerequisites are not ready";
  }
  return summary;
}

Objc3RuntimeMetadataSectionPublicationSummary
BuildRuntimeMetadataSectionPublicationSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary &runtime_metadata_section_abi,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  Objc3RuntimeMetadataSectionPublicationSummary summary;
  summary.fail_closed = true;
  if (!IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi) ||
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          runtime_export_enforcement)) {
    summary.failure_reason =
        "runtime metadata section publication prerequisites are not ready";
    return summary;
  }

  summary.publication_emitted = true;
  summary.uses_llvm_used = true;
  summary.image_info_emitted = true;
  summary.class_descriptor_count = runtime_export_legality.class_record_count;
  summary.protocol_descriptor_count =
      runtime_export_legality.protocol_record_count;
  summary.category_descriptor_count =
      runtime_export_legality.category_record_count;
  summary.property_descriptor_count =
      runtime_export_legality.property_record_count;
  summary.ivar_descriptor_count = runtime_export_legality.ivar_record_count;
  summary.total_descriptor_count =
      summary.class_descriptor_count + summary.protocol_descriptor_count +
      summary.category_descriptor_count + summary.property_descriptor_count +
      summary.ivar_descriptor_count;
  summary.total_retained_global_count = summary.total_descriptor_count + 6u;
  return summary;
}

Objc3RuntimeMetadataObjectInspectionHarnessSummary
BuildRuntimeMetadataObjectInspectionHarnessSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary &runtime_metadata_section_publication) {
  Objc3RuntimeMetadataObjectInspectionHarnessSummary summary;
  summary.fail_closed = true;
  if (!IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi) ||
      !IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
          runtime_metadata_section_publication)) {
    summary.failure_reason =
        "runtime metadata object inspection harness prerequisites are not ready";
    return summary;
  }

  summary.matrix_published = true;
  summary.uses_llvm_readobj = true;
  summary.uses_llvm_objdump = true;
  summary.matrix_row_count = 2u;
  return summary;
}

std::string BuildRuntimeMetadataSourceToSectionMatrixReplayKey(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_graph_contract=" << summary.source_graph_contract_id
      << ";section_abi_contract=" << summary.section_abi_contract_id
      << ";section_publication_contract=" << summary.section_publication_contract_id
      << ";object_inspection_contract=" << summary.object_inspection_contract_id
      << ";surface_path=" << summary.manifest_surface_path
      << ";row_ordering=" << summary.row_ordering_model
      << ";row_count=" << summary.matrix_row_count;
  for (const auto &row : summary.rows) {
    out << ";row=" << row.row_key << "|" << row.graph_node_kind << "|"
        << row.emission_mode << "|" << row.logical_section << "|"
        << row.payload_role << "|" << row.descriptor_symbol_family << "|"
        << row.aggregate_symbol << "|" << row.relocation_behavior << "|"
        << row.proof_fixture_path << "|" << row.proof_mode << "|"
        << row.section_inventory_command << "|" << row.symbol_inventory_command;
  }
  return out.str();
}

Objc3RuntimeMetadataSourceToSectionMatrixSummary
BuildRuntimeMetadataSourceToSectionMatrixSummary(
    const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph,
    const Objc3RuntimeMetadataSectionAbiFreezeSummary &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary &runtime_metadata_section_publication,
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        &runtime_metadata_object_inspection) {
  Objc3RuntimeMetadataSourceToSectionMatrixSummary summary;
  summary.matrix_published = true;
  summary.fail_closed = true;
  summary.source_graph_ready =
      IsReadyObjc3ExecutableMetadataSourceGraph(executable_metadata_source_graph);
  summary.section_abi_ready =
      IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi);
  summary.section_publication_ready =
      IsReadyObjc3RuntimeMetadataSectionPublicationSummary(
          runtime_metadata_section_publication);
  summary.object_inspection_ready =
      IsReadyObjc3RuntimeMetadataObjectInspectionHarnessSummary(
          runtime_metadata_object_inspection);
  summary.supported_node_coverage_complete = true;
  summary.explicit_non_goals_published = true;
  summary.row_ordering_frozen = true;
  summary.rows[0] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionInterfaceRowKey,
      "interface",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted interface payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[1] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionImplementationRowKey,
      "implementation",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted implementation payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[2] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionClassRowKey,
      "class",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalClassDescriptorSection,
      "standalone class descriptor payload",
      "__objc3_meta_class_####",
      kObjc3RuntimeMetadataClassDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[3] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionMetaclassRowKey,
      "metaclass",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted metaclass payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[4] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionProtocolRowKey,
      "protocol",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalProtocolDescriptorSection,
      "standalone protocol descriptor payload",
      "__objc3_meta_protocol_####",
      kObjc3RuntimeMetadataProtocolDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[5] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionCategoryRowKey,
      "category",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalCategoryDescriptorSection,
      "standalone category descriptor payload",
      "__objc3_meta_category_####",
      kObjc3RuntimeMetadataCategoryDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionCategoryFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[6] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionPropertyRowKey,
      "property",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalPropertyDescriptorSection,
      "standalone property descriptor payload",
      "__objc3_meta_property_####",
      kObjc3RuntimeMetadataPropertyDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[7] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionMethodRowKey,
      "method",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted method payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[8] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionIvarRowKey,
      "ivar",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalIvarDescriptorSection,
      "standalone ivar descriptor payload",
      "__objc3_meta_ivar_####",
      kObjc3RuntimeMetadataIvarDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.matrix_row_count = summary.rows.size();
  summary.replay_key =
      BuildRuntimeMetadataSourceToSectionMatrixReplayKey(summary);
  if (!IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(summary)) {
    summary.failure_reason =
        "runtime metadata source-to-section completeness matrix is incomplete";
  }
  return summary;
}

std::string BuildRuntimeMetadataSourceToSectionMatrixSummaryJson(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_graph_contract_id\":\""
      << EscapeJsonString(summary.source_graph_contract_id)
      << "\",\"section_abi_contract_id\":\""
      << EscapeJsonString(summary.section_abi_contract_id)
      << "\",\"section_publication_contract_id\":\""
      << EscapeJsonString(summary.section_publication_contract_id)
      << "\",\"object_inspection_contract_id\":\""
      << EscapeJsonString(summary.object_inspection_contract_id)
      << "\",\"manifest_surface_path\":\""
      << EscapeJsonString(summary.manifest_surface_path)
      << "\",\"row_ordering_model\":\""
      << EscapeJsonString(summary.row_ordering_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(summary)
              ? "true"
              : "false")
      << ",\"matrix_published\":"
      << (summary.matrix_published ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"source_graph_ready\":"
      << (summary.source_graph_ready ? "true" : "false")
      << ",\"section_abi_ready\":"
      << (summary.section_abi_ready ? "true" : "false")
      << ",\"section_publication_ready\":"
      << (summary.section_publication_ready ? "true" : "false")
      << ",\"object_inspection_ready\":"
      << (summary.object_inspection_ready ? "true" : "false")
      << ",\"supported_node_coverage_complete\":"
      << (summary.supported_node_coverage_complete ? "true" : "false")
      << ",\"explicit_non_goals_published\":"
      << (summary.explicit_non_goals_published ? "true" : "false")
      << ",\"row_ordering_frozen\":"
      << (summary.row_ordering_frozen ? "true" : "false")
      << ",\"matrix_row_count\":" << summary.matrix_row_count
      << ",\"rows\":[";
  for (std::size_t index = 0; index < summary.rows.size(); ++index) {
    const auto &row = summary.rows[index];
    if (index != 0u) {
      out << ",";
    }
    out << "{\"row_key\":\"" << EscapeJsonString(row.row_key)
        << "\",\"graph_node_kind\":\""
        << EscapeJsonString(row.graph_node_kind)
        << "\",\"emission_mode\":\""
        << EscapeJsonString(row.emission_mode)
        << "\",\"logical_section\":\""
        << EscapeJsonString(row.logical_section)
        << "\",\"payload_role\":\""
        << EscapeJsonString(row.payload_role)
        << "\",\"descriptor_symbol_family\":\""
        << EscapeJsonString(row.descriptor_symbol_family)
        << "\",\"aggregate_symbol\":\""
        << EscapeJsonString(row.aggregate_symbol)
        << "\",\"relocation_behavior\":\""
        << EscapeJsonString(row.relocation_behavior)
        << "\",\"proof_fixture_path\":\""
        << EscapeJsonString(row.proof_fixture_path)
        << "\",\"proof_mode\":\""
        << EscapeJsonString(row.proof_mode)
        << "\",\"section_inventory_command\":\""
        << EscapeJsonString(row.section_inventory_command)
        << "\",\"symbol_inventory_command\":\""
        << EscapeJsonString(row.symbol_inventory_command) << "\"}";
  }
  out << "],\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

Objc3RuntimeSupportLibraryContractSummary
BuildRuntimeSupportLibraryContractSummary() {
  Objc3RuntimeSupportLibraryContractSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.target_name_frozen = true;
  summary.exported_entrypoints_frozen = true;
  summary.ownership_boundaries_frozen = true;
  summary.build_constraints_frozen = true;
  summary.strict_dispatch_errors_required = true;
  summary.native_runtime_library_present = false;
  summary.driver_link_wiring_pending = true;
  summary.ready_for_runtime_library_skeleton = true;
  return summary;
}

Objc3RuntimeSupportLibraryCoreFeatureSummary
BuildRuntimeSupportLibraryCoreFeatureSummary(
    const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library) {
  Objc3RuntimeSupportLibraryCoreFeatureSummary summary;
  summary.support_library_contract_id = runtime_support_library.contract_id;
  summary.metadata_publication_contract_id =
      runtime_support_library.metadata_publication_contract_id;
  summary.fail_closed = true;
  summary.native_runtime_library_sources_present = true;
  summary.native_runtime_library_header_present = true;
  summary.native_runtime_library_archive_build_enabled = true;
  summary.native_runtime_library_entrypoints_implemented = true;
  summary.selector_lookup_stateful = true;
  summary.deterministic_dispatch_formula_matches_runtime_test_helper = true;
  summary.reset_for_testing_supported = true;
  summary.strict_dispatch_errors_required = true;
  summary.driver_link_wiring_pending = true;
  summary.ready_for_driver_link_wiring =
      IsReadyObjc3RuntimeSupportLibraryContractSummary(runtime_support_library);
  return summary;
}

Objc3RuntimeSupportLibraryLinkWiringSummary
BuildRuntimeSupportLibraryLinkWiringSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature) {
  Objc3RuntimeSupportLibraryLinkWiringSummary summary;
  summary.support_library_core_feature_contract_id =
      runtime_support_library_core_feature.contract_id;
  summary.fail_closed = true;
  summary.runtime_library_archive_available =
      runtime_support_library_core_feature
          .native_runtime_library_archive_build_enabled;
  summary.driver_emits_runtime_link_contract = true;
  summary.execution_smoke_consumes_runtime_library = true;
  summary.strict_dispatch_errors_required = true;
  summary.ready_for_runtime_library_consumption =
      IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
          runtime_support_library_core_feature);
  return summary;
}

std::string BuildRuntimeTranslationUnitRegistrationContractReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";binary_boundary_contract_id=" << summary.binary_boundary_contract_id
      << ";archive_static_link_contract_id="
      << summary.archive_static_link_contract_id
      << ";object_emission_closeout_contract_id="
      << summary.object_emission_closeout_contract_id
      << ";runtime_support_library_link_wiring_contract_id="
      << summary.runtime_support_library_link_wiring_contract_id
      << ";registration_surface_path=" << summary.registration_surface_path
      << ";registration_payload_model="
      << summary.registration_payload_model
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_root_ownership_model="
      << summary.constructor_root_ownership_model
      << ";constructor_emission_mode=" << summary.constructor_emission_mode
      << ";constructor_priority_policy="
      << summary.constructor_priority_policy
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";binary_boundary_replay_key=" << summary.binary_boundary_replay_key;
  for (std::size_t index = 0; index < summary.runtime_owned_payload_artifacts.size();
       ++index) {
    out << ";runtime_owned_payload_artifacts[" << index
        << "]=" << summary.runtime_owned_payload_artifacts[index];
  }
  return out.str();
}

Objc3RuntimeTranslationUnitRegistrationContractSummary
BuildRuntimeTranslationUnitRegistrationContractSummary(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
        &runtime_ingest_binary_boundary,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  Objc3RuntimeTranslationUnitRegistrationContractSummary summary;
  summary.fail_closed = true;
  summary.boundary_frozen = true;
  summary.binary_boundary_ready =
      IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
          runtime_ingest_binary_boundary);
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  const std::string archive_static_link_summary =
      Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary();
  summary.archive_static_link_surface_ready =
      archive_static_link_summary.find(
          std::string("contract=") +
          kObjc3RuntimeArchiveStaticLinkDiscoveryContractId) !=
          std::string::npos &&
      archive_static_link_summary.find(
          std::string("translation_unit_identity_model=") +
          summary.translation_unit_identity_model) != std::string::npos;
  const std::string object_emission_closeout_summary =
      Objc3RuntimeMetadataObjectEmissionCloseoutSummary();
  summary.object_emission_closeout_surface_ready =
      object_emission_closeout_summary.find(
          std::string("contract=") +
          kObjc3RuntimeMetadataObjectEmissionCloseoutContractId) !=
          std::string::npos &&
      object_emission_closeout_summary.find(
          "non_goals=no-startup-registration-or-runtime-bootstrap") !=
          std::string::npos;
  summary.runtime_owned_payload_inventory_published = true;
  summary.constructor_root_reserved_not_emitted = true;
  summary.startup_registration_not_yet_landed = true;
  summary.runtime_bootstrap_not_yet_landed = true;
  summary.explicit_non_goals_published = true;
  summary.runtime_owned_payload_artifact_count =
      summary.runtime_owned_payload_artifacts.size();
  if (summary.binary_boundary_ready) {
    summary.binary_boundary_replay_key = runtime_ingest_binary_boundary.replay_key;
  }
  summary.ready_for_registration_manifest_implementation =
      summary.binary_boundary_ready &&
      summary.archive_static_link_surface_ready &&
      summary.object_emission_closeout_surface_ready &&
      summary.runtime_support_library_link_wiring_ready;
  if (summary.ready_for_registration_manifest_implementation) {
    summary.replay_key =
        BuildRuntimeTranslationUnitRegistrationContractReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(summary)) {
    summary.failure_reason =
        "translation-unit registration surface contract is incomplete";
  }
  return summary;
}

std::string BuildRuntimeTranslationUnitRegistrationContractSummaryJson(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"binary_boundary_contract_id\":\""
      << EscapeJsonString(summary.binary_boundary_contract_id)
      << "\",\"archive_static_link_contract_id\":\""
      << EscapeJsonString(summary.archive_static_link_contract_id)
      << "\",\"object_emission_closeout_contract_id\":\""
      << EscapeJsonString(summary.object_emission_closeout_contract_id)
      << "\",\"runtime_support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(summary.runtime_support_library_link_wiring_contract_id)
      << "\",\"registration_surface_path\":\""
      << EscapeJsonString(summary.registration_surface_path)
      << "\",\"registration_payload_model\":\""
      << EscapeJsonString(summary.registration_payload_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(summary)
              ? "true"
              : "false")
      << ",\"boundary_frozen\":"
      << (summary.boundary_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"binary_boundary_ready\":"
      << (summary.binary_boundary_ready ? "true" : "false")
      << ",\"archive_static_link_surface_ready\":"
      << (summary.archive_static_link_surface_ready ? "true" : "false")
      << ",\"object_emission_closeout_surface_ready\":"
      << (summary.object_emission_closeout_surface_ready ? "true" : "false")
      << ",\"runtime_support_library_link_wiring_ready\":"
      << (summary.runtime_support_library_link_wiring_ready ? "true" : "false")
      << ",\"runtime_owned_payload_inventory_published\":"
      << (summary.runtime_owned_payload_inventory_published ? "true" : "false")
      << ",\"constructor_root_reserved_not_emitted\":"
      << (summary.constructor_root_reserved_not_emitted ? "true" : "false")
      << ",\"startup_registration_not_yet_landed\":"
      << (summary.startup_registration_not_yet_landed ? "true" : "false")
      << ",\"runtime_bootstrap_not_yet_landed\":"
      << (summary.runtime_bootstrap_not_yet_landed ? "true" : "false")
      << ",\"explicit_non_goals_published\":"
      << (summary.explicit_non_goals_published ? "true" : "false")
      << ",\"ready_for_registration_manifest_implementation\":"
      << (summary.ready_for_registration_manifest_implementation ? "true"
                                                                : "false")
      << ",\"runtime_owned_payload_artifact_count\":"
      << summary.runtime_owned_payload_artifact_count
      << ",\"runtime_owned_payload_artifacts\":[";
  for (std::size_t index = 0; index < summary.runtime_owned_payload_artifacts.size();
       ++index) {
    if (index != 0u) {
      out << ",";
    }
    out << "\""
        << EscapeJsonString(summary.runtime_owned_payload_artifacts[index])
        << "\"";
  }
  out << "],\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"constructor_root_ownership_model\":\""
      << EscapeJsonString(summary.constructor_root_ownership_model)
      << "\",\"constructor_emission_mode\":\""
      << EscapeJsonString(summary.constructor_emission_mode)
      << "\",\"constructor_priority_policy\":\""
      << EscapeJsonString(summary.constructor_priority_policy)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"binary_boundary_replay_key\":\""
      << EscapeJsonString(summary.binary_boundary_replay_key)
      << "\",\"replay_key\":\""
      << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeTranslationUnitRegistrationManifestReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";translation_unit_registration_contract_id="
      << summary.translation_unit_registration_contract_id
      << ";runtime_support_library_link_wiring_contract_id="
      << summary.runtime_support_library_link_wiring_contract_id
      << ";manifest_surface_path=" << summary.manifest_surface_path
      << ";manifest_payload_model=" << summary.manifest_payload_model
      << ";manifest_artifact_relative_path="
      << summary.manifest_artifact_relative_path
      << ";runtime_support_library_archive_relative_path="
      << summary.runtime_support_library_archive_relative_path
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_root_ownership_model="
      << summary.constructor_root_ownership_model
      << ";manifest_authority_model=" << summary.manifest_authority_model
      << ";constructor_init_stub_symbol_prefix="
      << summary.constructor_init_stub_symbol_prefix
      << ";constructor_init_stub_ownership_model="
      << summary.constructor_init_stub_ownership_model
      << ";constructor_priority_policy="
      << summary.constructor_priority_policy
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";class_descriptor_count=" << summary.class_descriptor_count
      << ";protocol_descriptor_count=" << summary.protocol_descriptor_count
      << ";category_descriptor_count=" << summary.category_descriptor_count
      << ";property_descriptor_count=" << summary.property_descriptor_count
      << ";ivar_descriptor_count=" << summary.ivar_descriptor_count
      << ";total_descriptor_count=" << summary.total_descriptor_count
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";translation_unit_registration_replay_key="
      << summary.translation_unit_registration_replay_key;
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    out << ";runtime_owned_payload_artifacts[" << index
        << "]=" << summary.runtime_owned_payload_artifacts[index];
  }
  return out.str();
}

Objc3RuntimeTranslationUnitRegistrationManifestSummary
BuildRuntimeTranslationUnitRegistrationManifestSummary(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary
        &registration_contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    std::uint64_t translation_unit_registration_order_ordinal) {
  Objc3RuntimeTranslationUnitRegistrationManifestSummary summary;
  summary.fail_closed = true;
  summary.translation_unit_registration_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(
          registration_contract);
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  summary.runtime_manifest_template_published = true;
  summary.constructor_root_manifest_authoritative = true;
  summary.constructor_root_reserved_for_lowering = true;
  summary.init_stub_emission_deferred_to_lowering = true;
  summary.runtime_registration_artifact_emitted_by_driver = true;
  summary.runtime_owned_payload_artifact_count =
      summary.runtime_owned_payload_artifacts.size();
  summary.class_descriptor_count =
      runtime_metadata_section_publication.class_descriptor_count;
  summary.protocol_descriptor_count =
      runtime_metadata_section_publication.protocol_descriptor_count;
  summary.category_descriptor_count =
      runtime_metadata_section_publication.category_descriptor_count;
  summary.property_descriptor_count =
      runtime_metadata_section_publication.property_descriptor_count;
  summary.ivar_descriptor_count =
      runtime_metadata_section_publication.ivar_descriptor_count;
  summary.total_descriptor_count =
      runtime_metadata_section_publication.total_descriptor_count;
  if (translation_unit_registration_order_ordinal > 0) {
    summary.translation_unit_registration_order_ordinal =
        translation_unit_registration_order_ordinal;
  }
  if (summary.translation_unit_registration_contract_ready) {
    summary.translation_unit_registration_replay_key =
        registration_contract.replay_key;
  }
  summary.ready_for_lowering_init_stub_emission =
      summary.translation_unit_registration_contract_ready &&
      summary.runtime_support_library_link_wiring_ready;
  summary.launch_integration_ready =
      summary.ready_for_lowering_init_stub_emission;
  if (summary.ready_for_lowering_init_stub_emission) {
    summary.replay_key =
        BuildRuntimeTranslationUnitRegistrationManifestReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(summary)) {
    summary.failure_reason =
        "translation-unit registration manifest summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeTranslationUnitRegistrationManifestSummaryJson(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"launch_integration_contract_id\":\""
      << EscapeJsonString(summary.launch_integration_contract_id)
      << "\",\"translation_unit_registration_contract_id\":\""
      << EscapeJsonString(summary.translation_unit_registration_contract_id)
      << "\",\"runtime_support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(
             summary.runtime_support_library_link_wiring_contract_id)
      << "\",\"manifest_surface_path\":\""
      << EscapeJsonString(summary.manifest_surface_path)
      << "\",\"manifest_payload_model\":\""
      << EscapeJsonString(summary.manifest_payload_model)
      << "\",\"manifest_artifact_relative_path\":\""
      << EscapeJsonString(summary.manifest_artifact_relative_path)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"translation_unit_registration_contract_ready\":"
      << (summary.translation_unit_registration_contract_ready ? "true"
                                                              : "false")
      << ",\"runtime_support_library_link_wiring_ready\":"
      << (summary.runtime_support_library_link_wiring_ready ? "true"
                                                            : "false")
      << ",\"runtime_manifest_template_published\":"
      << (summary.runtime_manifest_template_published ? "true" : "false")
      << ",\"constructor_root_manifest_authoritative\":"
      << (summary.constructor_root_manifest_authoritative ? "true" : "false")
      << ",\"constructor_root_reserved_for_lowering\":"
      << (summary.constructor_root_reserved_for_lowering ? "true" : "false")
      << ",\"init_stub_emission_deferred_to_lowering\":"
      << (summary.init_stub_emission_deferred_to_lowering ? "true" : "false")
      << ",\"runtime_registration_artifact_emitted_by_driver\":"
      << (summary.runtime_registration_artifact_emitted_by_driver ? "true"
                                                                 : "false")
      << ",\"ready_for_lowering_init_stub_emission\":"
      << (summary.ready_for_lowering_init_stub_emission ? "true" : "false")
      << ",\"launch_integration_ready\":"
      << (summary.launch_integration_ready ? "true" : "false")
      << ",\"runtime_owned_payload_artifact_count\":"
      << summary.runtime_owned_payload_artifact_count
      << ",\"runtime_owned_payload_artifacts\":[";
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    if (index != 0u) {
      out << ",";
    }
    out << "\""
        << EscapeJsonString(summary.runtime_owned_payload_artifacts[index])
        << "\"";
  }
  out << "],\"runtime_support_library_archive_relative_path\":\""
      << EscapeJsonString(summary.runtime_support_library_archive_relative_path)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"constructor_root_ownership_model\":\""
      << EscapeJsonString(summary.constructor_root_ownership_model)
      << "\",\"manifest_authority_model\":\""
      << EscapeJsonString(summary.manifest_authority_model)
      << "\",\"constructor_init_stub_symbol_prefix\":\""
      << EscapeJsonString(summary.constructor_init_stub_symbol_prefix)
      << "\",\"constructor_init_stub_ownership_model\":\""
      << EscapeJsonString(summary.constructor_init_stub_ownership_model)
      << "\",\"constructor_priority_policy\":\""
      << EscapeJsonString(summary.constructor_priority_policy)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"runtime_library_resolution_model\":\""
      << EscapeJsonString(summary.runtime_library_resolution_model)
      << "\",\"driver_linker_flag_consumption_model\":\""
      << EscapeJsonString(summary.driver_linker_flag_consumption_model)
      << "\",\"compile_wrapper_command_surface\":\""
      << EscapeJsonString(summary.compile_wrapper_command_surface)
      << "\",\"compile_proof_command_surface\":\""
      << EscapeJsonString(summary.compile_proof_command_surface)
      << "\",\"execution_smoke_command_surface\":\""
      << EscapeJsonString(summary.execution_smoke_command_surface)
      << "\",\"class_descriptor_count\":"
      << summary.class_descriptor_count
      << ",\"protocol_descriptor_count\":"
      << summary.protocol_descriptor_count
      << ",\"category_descriptor_count\":"
      << summary.category_descriptor_count
      << ",\"property_descriptor_count\":"
      << summary.property_descriptor_count
      << ",\"ivar_descriptor_count\":"
      << summary.ivar_descriptor_count
      << ",\"total_descriptor_count\":"
      << summary.total_descriptor_count
      << ",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"translation_unit_registration_replay_key\":\""
      << EscapeJsonString(summary.translation_unit_registration_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeStartupBootstrapInvariantReplayKey(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";realization_order_policy=" << summary.realization_order_policy
      << ";failure_mode=" << summary.failure_mode
      << ";image_local_initialization_scope="
      << summary.image_local_initialization_scope
      << ";constructor_root_uniqueness_policy="
      << summary.constructor_root_uniqueness_policy
      << ";constructor_root_consumption_model="
      << summary.constructor_root_consumption_model
      << ";startup_execution_mode=" << summary.startup_execution_mode
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";manifest_authority_model=" << summary.manifest_authority_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key;
  return out.str();
}

Objc3RuntimeStartupBootstrapInvariantSummary
BuildRuntimeStartupBootstrapInvariantSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest) {
  Objc3RuntimeStartupBootstrapInvariantSummary summary;
  summary.fail_closed = true;
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.duplicate_registration_semantics_frozen = true;
  summary.realization_order_semantics_frozen = true;
  summary.failure_mode_semantics_frozen = true;
  summary.image_local_initialization_scope_frozen = true;
  summary.constructor_root_uniqueness_frozen = true;
  summary.startup_execution_not_yet_landed = true;
  summary.live_duplicate_registration_enforcement_not_yet_landed = true;
  summary.image_local_realization_not_yet_landed = true;
  summary.ready_for_bootstrap_implementation =
      summary.registration_manifest_contract_ready;
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.ready_for_bootstrap_implementation) {
    summary.replay_key =
        BuildRuntimeStartupBootstrapInvariantReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(summary)) {
    summary.failure_reason =
        "runtime startup bootstrap invariant summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeStartupBootstrapInvariantSummaryJson(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"duplicate_registration_policy\":\""
      << EscapeJsonString(summary.duplicate_registration_policy)
      << "\",\"realization_order_policy\":\""
      << EscapeJsonString(summary.realization_order_policy)
      << "\",\"failure_mode\":\""
      << EscapeJsonString(summary.failure_mode)
      << "\",\"image_local_initialization_scope\":\""
      << EscapeJsonString(summary.image_local_initialization_scope)
      << "\",\"constructor_root_uniqueness_policy\":\""
      << EscapeJsonString(summary.constructor_root_uniqueness_policy)
      << "\",\"constructor_root_consumption_model\":\""
      << EscapeJsonString(summary.constructor_root_consumption_model)
      << "\",\"startup_execution_mode\":\""
      << EscapeJsonString(summary.startup_execution_mode)
      << "\",\"constructor_root_symbol\":\""
