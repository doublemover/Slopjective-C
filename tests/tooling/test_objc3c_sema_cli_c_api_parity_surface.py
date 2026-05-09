import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FRONTEND_ANCHOR = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
DIAG_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "io" / "objc3_diagnostics_artifacts.cpp"
PIPELINE_ARTIFACTS = ROOT / "native" / "objc3c" / "src" / "artifacts" / "objc3_frontend_artifacts.cpp"
FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "native"


def _read(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    expanded: list[str] = []
    for line in text.splitlines():
        expanded.append(line)
        stripped = line.strip()
        if not stripped.startswith('#include "'):
            continue
        include_path = stripped.split('"', 2)[1]
        target = ROOT / "native" / "objc3c" / "src" / include_path
        if target.exists():
            expanded.append(target.read_text(encoding="utf-8"))
    return "\n".join(expanded)


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_c_api_and_cli_diagnostics_json_surface_match() -> None:
    frontend_anchor = _read(FRONTEND_ANCHOR)
    diag_artifacts = _read(DIAG_ARTIFACTS)

    assert "ParseFrontendDiagnostic(" in frontend_anchor
    assert '\\"severity\\":\\"' in frontend_anchor
    assert '\\"line\\":' in frontend_anchor
    assert '\\"column\\":' in frontend_anchor
    assert '\\"code\\":\\"' in frontend_anchor
    assert '\\"message\\":\\"' in frontend_anchor
    assert '\\"raw\\":\\"' in frontend_anchor

    assert '\\"severity\\":\\"' in diag_artifacts
    assert '\\"line\\":' in diag_artifacts
    assert '\\"column\\":' in diag_artifacts
    assert '\\"code\\":\\"' in diag_artifacts
    assert '\\"message\\":\\"' in diag_artifacts
    assert '\\"raw\\":\\"' in diag_artifacts


def test_manifest_emits_sema_parity_contract_fields() -> None:
    artifacts = _read(PIPELINE_ARTIFACTS)

    assert "language_version" in artifacts
    assert "language_profile" in artifacts
    assert "canonical_literal_rejection_diagnostics" in artifacts
    _assert_in_order(
        artifacts,
        [
            'manifest << "  \\"frontend\\": {\\n";',
            'manifest << "    \\"language_version\\":"',
            'manifest << "    \\"language_profile\\":\\""',
            'manifest << "    \\"canonical_literal_rejection_diagnostics\\":true,',
            'manifest << "    \\"max_message_send_args\\":"',
        ],
    )
    assert "diagnostics_after_build" in artifacts
    assert "diagnostics_after_validate_bodies" in artifacts
    assert "diagnostics_after_validate_pure_contract" in artifacts
    assert "diagnostics_emitted_by_build" in artifacts
    assert "diagnostics_emitted_by_validate_bodies" in artifacts
    assert "diagnostics_emitted_by_validate_pure_contract" in artifacts
    assert "diagnostics_monotonic" in artifacts
    assert "deterministic_semantic_diagnostics" in artifacts
    assert "deterministic_type_metadata_handoff" in artifacts
    assert "parity_ready" in artifacts
    assert "type_metadata_global_entries" in artifacts
    assert "type_metadata_function_entries" in artifacts


def test_frontend_c_api_contract_fixtures_pin_owner_truth_surfaces() -> None:
    runner_contract = json.loads(
        (FIXTURE_ROOT / "frontend_c_api_runner_contract.json").read_text(
            encoding="utf-8"
        )
    )
    helper_contract = json.loads(
        (FIXTURE_ROOT / "frontend_c_api_helper_contract.json").read_text(
            encoding="utf-8"
        )
    )

    assert runner_contract["owner_contract"] == {
        "runner_owner": "frontend-c-api-runner",
        "helper_owner": "frontend-c-api-helper-contract",
        "result_owner": "frontend-c-api-runner-result",
        "artifact_owner": "frontend-c-api-runner-artifact",
        "status_owner": "frontend-c-api-runner-status",
        "no_fallback_or_report_only_claims": True,
    }
    assert helper_contract["owner_contract"] == {
        "helper_owner": "frontend-c-api-helper-contract",
        "runner_owner": "frontend-c-api-runner",
        "result_lifecycle_owner": "frontend-c-api-result-lifecycle",
        "string_owner": "frontend-c-api-string-lifecycle",
        "diagnostics_owner": "frontend-c-api-diagnostics-artifact",
        "result_owner": "frontend-c-api-helper-result-accessor",
        "artifact_owner": "frontend-c-api-helper-artifact-accessor",
        "status_owner": "frontend-c-api-helper-stage-status",
        "null_invalid_input_owner": "frontend-c-api-null-invalid-input",
        "abi_version_owner": "frontend-c-api-abi-version",
        "public_private_partition_owner": "frontend-c-api-public-private-partition",
        "no_fallback_or_report_only_claims": True,
    }
