from __future__ import annotations

import shutil
import subprocess
from copy import deepcopy
from pathlib import Path

import pytest

import scripts.ingest_objc3c_platform_host_evidence as host_evidence_ingest
from scripts.check_platform_host_promotion_evidence import (
    REQUIRED_BLOCKER_FAILURE_CLASSES,
    REQUIRED_GATE_CLASSES,
    REQUIRED_PLATFORM_IDS,
    load_host_promotion_evidence_contract,
    load_host_promotion_reviewed_source_inputs,
    validate_host_promotion_evidence_contract,
    validate_host_promotion_reviewed_source_inputs,
)
from scripts.ingest_objc3c_platform_host_evidence import (
    require_installed_root_execution_record,
)
from scripts.platform_hardening_contracts.host_promotion import (
    HOST_PROMOTION_PACKAGE_CHANNEL_LAYOUT_BY_PLATFORM,
    HOST_PROMOTION_REQUIRED_HOSTED_PROMOTION_ARTIFACT_SUFFIXES,
)


INSTALL_PREFIX_LAYOUT_BY_PLATFORM = {
    "linux-x64": [
        "bin/objc3c-native",
        "lib/libobjc3-runtime.so",
        "include/objc3/runtime",
    ],
    "darwin-arm64": [
        "bin/objc3c-native",
        "lib/libobjc3-runtime.dylib",
        "include/objc3/runtime",
    ],
}


def _installed_root_execution_record(channel_id: str, platform_id: str) -> dict[str, object]:
    return {
        "contract_id": "objc3c.packaging.channels.installed-root-native-execution.v1",
        "status": "PASS",
        "channel_id": channel_id,
        "target_platform_id": platform_id,
        "execution_source": "installed-root",
        "repo_temp_dependency": False,
        "preexisting_artifacts_dependency": False,
        "returncode": 2,
        "usage_banner_seen": True,
    }


def _platform_artifact_paths(platform_id: str) -> list[str]:
    return [
        f"tmp/reports/platform-host-evidence/{platform_id}/{suffix}"
        for suffix in HOST_PROMOTION_REQUIRED_HOSTED_PROMOTION_ARTIFACT_SUFFIXES
    ]


def _mark_record_as_reviewed_source(
    record: dict[str, object],
    platform_id: str,
    *,
    include_artifacts: bool = True,
) -> None:
    record["claim_state"] = "reviewed-source"
    record["promotion_allowed"] = True
    record["platform_ids"] = [platform_id]
    record["review_status"] = "reviewed-current-source"
    record["stale_evidence_allowed"] = False
    record["prose_only_evidence"] = False
    record["local_temp_evidence_claim"] = False
    record["source_paths"] = [
        "tests/tooling/fixtures/platform_hardening/host_promotion_reviewed_source_inputs.json",
        "tests/tooling/fixtures/platform_hardening/platform_host_promotion_evidence_contract.json",
        "tests/tooling/fixtures/platform_hardening/platform_toolchain_support_evidence.json",
        "tests/tooling/fixtures/platform_support/source_truth_matrix.json",
    ]
    if include_artifacts:
        paths = _platform_artifact_paths(platform_id)
        record["hosted_runner_artifact_paths"] = paths
        record["toolchain_artifact_paths"] = paths
        record["package_artifact_paths"] = paths


def test_platform_host_promotion_evidence_fixture_validates() -> None:
    payload = load_host_promotion_evidence_contract()

    summary = validate_host_promotion_evidence_contract(payload)

    assert summary == {
        "contract_id": "objc3c.platform.host-promotion.evidence.summary.v1",
        "status": "PASS",
        "source_contract_id": "objc3c.platform.host-promotion.evidence.contract.v1",
        "source_path": (
            "tests/tooling/fixtures/platform_hardening/"
            "platform_host_promotion_evidence_contract.json"
        ),
        "platform_ids": list(REQUIRED_PLATFORM_IDS),
        "required_gate_classes": list(REQUIRED_GATE_CLASSES),
        "required_source_record_types": [
            "host_identity",
            "toolchain_probe",
            "package_root",
            "install_receipt",
            "native_execution",
            "object_identity",
            "debug_identity",
            "package_install_identity",
            "runtime_load_link_proof",
        ],
        "required_blocker_failure_classes": list(REQUIRED_BLOCKER_FAILURE_CLASSES),
        "required_hosted_evidence": True,
        "prose_only_platform_support_claims_allowed": False,
        "summary_path": "tmp/reports/platform-hardening/host-promotion-evidence-summary.json",
        "promotion_allowed": False,
    }

    platforms = {row["platform_id"]: row for row in payload["platforms"]}
    assert set(platforms) == {"linux-x64", "darwin-arm64"}
    assert platforms["linux-x64"]["package_install_native_execution_evidence"][
        "package_root_layout"
    ] == list(HOST_PROMOTION_PACKAGE_CHANNEL_LAYOUT_BY_PLATFORM["linux-x64"])
    assert platforms["darwin-arm64"]["package_install_native_execution_evidence"][
        "package_root_layout"
    ] == list(HOST_PROMOTION_PACKAGE_CHANNEL_LAYOUT_BY_PLATFORM["darwin-arm64"])
    assert platforms["linux-x64"]["package_install_native_execution_evidence"][
        "installed_root_execution_summary_path"
    ] == "tmp/reports/platform-host-evidence/linux-x64/install/end-to-end-summary.json"
    assert platforms["darwin-arm64"]["package_install_native_execution_evidence"][
        "installed_root_execution_summary_path"
    ] == "tmp/reports/platform-host-evidence/darwin-arm64/install/end-to-end-summary.json"
    assert HOST_PROMOTION_PACKAGE_CHANNEL_LAYOUT_BY_PLATFORM["windows-x64"][1:3] == (
        "artifacts/bin/objc3c-native.exe",
        "artifacts/lib/objc3_runtime.lib",
    )
    assert platforms["linux-x64"]["object_debug_identity"] == {
        "object_format": "ELF",
        "debug_format": "DWARF",
        "target_triple": "x86_64-unknown-linux-gnu",
        "arch": "x64",
        "object_identity_report_path": (
            "tmp/reports/platform-host-evidence/linux-x64/build/object-identity.json"
        ),
        "debug_identity_report_path": (
            "tmp/reports/platform-host-evidence/linux-x64/build/debug-identity.json"
        ),
        "llvm_toolchain_record_id": "objc3c.host.linux-x64.toolchain-probes.fail-closed",
        "wrong_format_behavior": "fail-closed-before-package-publication",
        "wrong_arch_behavior": "fail-closed-before-install",
        "platform_specific_debug_proofs": {},
        "support_truth_without_package_install_execution": False,
    }
    assert platforms["darwin-arm64"]["object_debug_identity"] == {
        "object_format": "Mach-O",
        "debug_format": "DWARF/dSYM",
        "target_triple": "aarch64-apple-darwin",
        "arch": "arm64",
        "object_identity_report_path": (
            "tmp/reports/platform-host-evidence/darwin-arm64/build/object-identity.json"
        ),
        "debug_identity_report_path": (
            "tmp/reports/platform-host-evidence/darwin-arm64/build/debug-identity.json"
        ),
        "llvm_toolchain_record_id": (
            "objc3c.host.darwin-arm64.toolchain-probes.fail-closed"
        ),
        "wrong_format_behavior": "fail-closed-before-package-publication",
        "wrong_arch_behavior": "fail-closed-before-install",
        "platform_specific_debug_proofs": {
            "dsym_uuid_required": True,
            "dsym_uuid_arch": "arm64",
            "dsym_uuid_match_required": True,
            "required_debug_proofs": [
                "mach_o_arm64_architecture",
                "binary_dsym_uuid",
                "dsym_uuid_arch_arm64",
                "binary_dsym_uuid_match",
            ],
            "debug_proof_failure_behavior": "fail-closed-before-package-publication",
        },
        "support_truth_without_package_install_execution": False,
    }
    assert platforms["linux-x64"]["runtime_link_load_identity"][
        "platform_specific_runtime_proofs"
    ] == {}
    assert platforms["darwin-arm64"]["runtime_link_load_identity"][
        "platform_specific_runtime_proofs"
    ] == {
        "install_name_required": True,
        "rpath_required": True,
        "codesign_required": True,
        "expected_arch": "arm64",
        "load_commands_required": [
            "LC_ID_DYLIB",
            "LC_RPATH",
            "LC_LOAD_DYLIB",
        ],
        "executable_runtime_reference_required": True,
        "runtime_proof_failure_behavior": "fail-closed-before-native-execution-claim",
    }


def test_platform_host_promotion_evidence_rejects_missing_linux_row() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"] = [
        row for row in payload["platforms"] if row["platform_id"] != "linux-x64"
    ]

    with pytest.raises(RuntimeError, match="missing required platform rows: linux-x64"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_generated_promotion() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["generated_hosted_evidence"]["support_truth"] = True

    with pytest.raises(RuntimeError, match="linux-x64 generated evidence promoted support"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_unrequired_hosted_evidence() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["generated_hosted_evidence"]["required"] = False

    with pytest.raises(RuntimeError, match="linux-x64 hosted evidence is not required"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_prose_only_support_claims() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["support_claim_policy"][
        "prose_only_platform_support_claims_allowed"
    ] = True

    with pytest.raises(RuntimeError, match="linux-x64 allowed prose-only support claims"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_non_tmp_report_output() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["generated_hosted_evidence"][
        "generated_report_root"
    ] = "artifacts/platform-host-evidence/linux-x64"

    with pytest.raises(RuntimeError, match="generated report root must write under tmp/reports"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_reviewed_source_truth() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["reviewed_source_truth_evidence"]["required"] = False

    with pytest.raises(RuntimeError, match="linux-x64 reviewed source truth is not required"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_execution_gate() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["reviewed_source_truth_evidence"]["missing_record_classes"] = [
        "build",
        "package",
        "install",
    ]

    with pytest.raises(
        RuntimeError,
        match="missing build/package/install/execution gates: execution",
    ):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_linux_object_format_drift() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["object_debug_identity"]["object_format"] = "COFF"

    with pytest.raises(RuntimeError, match="linux-x64 object/debug identity object_format drifted"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_darwin_debug_format_drift() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][1]["object_debug_identity"]["debug_format"] = "DWARF"

    with pytest.raises(
        RuntimeError,
        match="darwin-arm64 object/debug identity debug_format drifted",
    ):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_runtime_load_requirement() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][1]["runtime_link_load_identity"]["runtime_load_probe_required"] = False

    with pytest.raises(RuntimeError, match="darwin-arm64 runtime load probe is not mandatory"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_package_root_layout() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["package_install_native_execution_evidence"][
        "package_root_layout"
    ] = []

    with pytest.raises(RuntimeError, match="package_root_layout must not be empty"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_install_prefix_package_root_layout() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["package_install_native_execution_evidence"][
        "package_root_layout"
    ] = INSTALL_PREFIX_LAYOUT_BY_PLATFORM["linux-x64"]

    with pytest.raises(RuntimeError, match="package_root_layout used install-prefix paths"):
        validate_host_promotion_evidence_contract(payload)


def test_reviewed_source_inputs_reject_install_prefix_package_root_layout() -> None:
    payload = deepcopy(load_host_promotion_reviewed_source_inputs())
    payload["package_root_evidence_records"][0][
        "package_root_layout"
    ] = INSTALL_PREFIX_LAYOUT_BY_PLATFORM["linux-x64"]

    with pytest.raises(RuntimeError, match="package_root_layout used install-prefix paths"):
        validate_host_promotion_reviewed_source_inputs(payload)


def test_reviewed_source_inputs_reject_missing_durable_fixture_path() -> None:
    payload = deepcopy(load_host_promotion_reviewed_source_inputs())
    payload["platforms"][0]["reviewed_source_paths"] = [
        "tests/tooling/fixtures/platform_hardening/host_promotion_reviewed_source_inputs.json"
    ]

    with pytest.raises(RuntimeError, match="missing durable reviewed source fixture paths"):
        validate_host_promotion_reviewed_source_inputs(payload)


def test_reviewed_source_inputs_reject_stale_promotion_record() -> None:
    payload = deepcopy(load_host_promotion_reviewed_source_inputs())
    record = payload["host_identity_records"][0]
    _mark_record_as_reviewed_source(record, "linux-x64")
    record["stale_evidence_allowed"] = True

    with pytest.raises(RuntimeError, match="allowed stale evidence"):
        validate_host_promotion_reviewed_source_inputs(payload)


def test_reviewed_source_inputs_reject_prose_only_promotion_record() -> None:
    payload = deepcopy(load_host_promotion_reviewed_source_inputs())
    record = payload["host_identity_records"][0]
    _mark_record_as_reviewed_source(record, "linux-x64")
    record["prose_only_evidence"] = True

    with pytest.raises(RuntimeError, match="used prose-only evidence"):
        validate_host_promotion_reviewed_source_inputs(payload)


def test_reviewed_source_inputs_reject_local_temp_promotion_claim() -> None:
    payload = deepcopy(load_host_promotion_reviewed_source_inputs())
    record = payload["host_identity_records"][0]
    _mark_record_as_reviewed_source(record, "linux-x64")
    record["local_temp_evidence_claim"] = True

    with pytest.raises(RuntimeError, match="used local temp evidence claim"):
        validate_host_promotion_reviewed_source_inputs(payload)


def test_reviewed_source_inputs_require_hosted_toolchain_package_artifacts() -> None:
    payload = deepcopy(load_host_promotion_reviewed_source_inputs())
    record = payload["host_identity_records"][0]
    _mark_record_as_reviewed_source(record, "linux-x64", include_artifacts=False)

    with pytest.raises(RuntimeError, match="missing list field hosted_runner_artifact_paths"):
        validate_host_promotion_reviewed_source_inputs(payload)


def test_platform_host_promotion_evidence_rejects_missing_install_receipt_requirement() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["package_install_native_execution_evidence"][
        "install_receipt_required"
    ] = False

    with pytest.raises(RuntimeError, match="linux-x64 install receipt is not mandatory"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_installed_root_execution_requirement() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["package_install_native_execution_evidence"][
        "installed_root_execution_required"
    ] = False

    with pytest.raises(RuntimeError, match="linux-x64 installed-root execution is not mandatory"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_missing_offline_installed_root_execution_requirement() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["package_install_native_execution_evidence"][
        "offline_installed_root_execution_required"
    ] = False

    with pytest.raises(RuntimeError, match="linux-x64 offline installed-root execution is not mandatory"):
        validate_host_promotion_evidence_contract(payload)


def test_host_evidence_ingestion_accepts_installed_root_execution_record() -> None:
    payload = {
        "status": "generated-host-artifact-present",
        "installed_root_execution": _installed_root_execution_record(
            "local-installer",
            "linux-x64",
        ),
    }

    require_installed_root_execution_record(
        payload,
        field_name="installed_root_execution",
        expected_channel_id="local-installer",
        platform_id="linux-x64",
        owner="test-owner",
    )


def test_host_evidence_ingestion_rejects_missing_installed_root_execution_record() -> None:
    with pytest.raises(RuntimeError, match="installed_root_execution must be an object"):
        require_installed_root_execution_record(
            {"status": "generated-host-artifact-present"},
            field_name="installed_root_execution",
            expected_channel_id="local-installer",
            platform_id="linux-x64",
            owner="test-owner",
        )


def test_host_evidence_ingestion_rejects_repo_temp_installed_root_execution_dependency() -> None:
    record = _installed_root_execution_record("offline-bundle", "linux-x64")
    record["repo_temp_dependency"] = True
    payload = {
        "status": "generated-host-artifact-present",
        "offline_installed_root_execution": record,
    }

    with pytest.raises(RuntimeError, match="depended on repo temp output"):
        require_installed_root_execution_record(
            payload,
            field_name="offline_installed_root_execution",
            expected_channel_id="offline-bundle",
            platform_id="linux-x64",
            owner="test-owner",
        )


def test_platform_host_promotion_evidence_rejects_missing_blocker_case() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["common_fail_closed_blockers"] = [
        row
        for row in payload["common_fail_closed_blockers"]
        if row["failure_class"] != "sanitizer-variant-leakage"
    ]

    with pytest.raises(
        RuntimeError,
        match="missing required blocker failure classes: sanitizer-variant-leakage",
    ):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_sanitizer_leakage_boundary_drift() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["platforms"][0]["sanitizer_variant_boundary"][
        "release_runtime_allows_sanitizer_artifacts"
    ] = True

    with pytest.raises(RuntimeError, match="linux-x64 release runtime allowed sanitizer artifacts"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_public_command_drift() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["future_checker_contract"][
        "must_register_public_command"
    ] = "ingest-platform-host-evidence"

    with pytest.raises(RuntimeError, match="future checker public command drifted"):
        validate_host_promotion_evidence_contract(payload)


def test_platform_host_promotion_evidence_rejects_integration_parent_drift() -> None:
    payload = deepcopy(load_host_promotion_evidence_contract())
    payload["future_checker_contract"][
        "must_be_child_of_public_action"
    ] = "validate-packaging-channels"

    with pytest.raises(RuntimeError, match="future checker integration parent action drifted"):
        validate_host_promotion_evidence_contract(payload)


def test_host_evidence_fail_closed_placeholder_is_not_promotion_ready(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path,
) -> None:
    monkeypatch.setattr(host_evidence_ingest, "ROOT", tmp_path)
    scoped_path = (
        "tmp/reports/platform-host-evidence/linux-x64/build/native_build_summary.json"
    )

    artifact = host_evidence_ingest.materialize_generated_artifact(
        "tmp/build-objc3c-native/native_build_summary.json",
        scoped_path,
        platform_id="linux-x64",
        step_id="build",
        evidence_class="build",
        outcome="failure",
    )

    assert artifact["exists"] is True
    assert artifact["fail_closed_placeholder"] is True
    assert artifact["promotion_usable"] is False
    assert artifact["required_source_artifacts"] == [
        "tmp/build-objc3c-native/native_build_summary.json"
    ]
    assert artifact["diagnostics"]["classification"] == "incomplete-review-candidate"
    placeholder = host_evidence_ingest.load_platform_generated_json(
        "linux-x64",
        "build/native_build_summary.json",
    )
    assert placeholder["source_artifacts"] == [
        {
            "path": "tmp/build-objc3c-native/native_build_summary.json",
            "exists": False,
        }
    ]
    assert placeholder["diagnostics"]["required_source_artifacts"] == [
        "tmp/build-objc3c-native/native_build_summary.json"
    ]
    host_evidence_ingest.require_fail_closed_placeholder(
        placeholder,
        platform_id="linux-x64",
        owner=scoped_path,
    )


def test_host_evidence_review_candidate_rejects_fail_closed_placeholders(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path,
) -> None:
    monkeypatch.setattr(host_evidence_ingest, "ROOT", tmp_path)
    for suffix in (
        "package/objc3c-runnable-toolchain-package.json",
        "package/runtime-library-manifest.json",
    ):
        host_evidence_ingest.write_fail_closed_placeholder_artifact(
            platform_id="linux-x64",
            step_id="package",
            evidence_class="package",
            outcome="failure",
            source_path_text=suffix,
            scoped_path_text=f"tmp/reports/platform-host-evidence/linux-x64/{suffix}",
        )

    candidate = host_evidence_ingest.build_review_candidate_source_truth(
        "linux-x64",
        workflow_path=".github/workflows/conformance-minima.yml",
        runner_label="ubuntu-24.04",
    )

    package_rows = [
        row
        for row in candidate["review_candidate_rows"]
        if row["record_type"] == "package_root"
    ]
    assert len(package_rows) == 1
    assert package_rows[0]["generated_artifacts_complete"] is False
    assert package_rows[0]["required_source_artifacts"] == [
        "package/objc3c-runnable-toolchain-package.json",
        "package/runtime-library-manifest.json",
    ]
    assert [
        diagnostic["status"]
        for diagnostic in package_rows[0]["incomplete_diagnostics"]
    ] == [
        "producer-failed-before-success-artifact",
        "producer-failed-before-success-artifact",
    ]
    assert all(
        artifact.get("fail_closed_placeholder") is True
        for artifact in package_rows[0]["generated_artifacts"]
    )


def test_host_evidence_incomplete_runtime_manifest_keeps_review_candidate(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path,
) -> None:
    monkeypatch.setattr(host_evidence_ingest, "ROOT", tmp_path)
    payload = {
        "contract_id": "objc3c.platform.hosted-runtime-library-manifest.generated.v1",
        "schema_version": 1,
        "platform_id": "linux-x64",
        "issue_ref": 8228,
        "generated_report_path": (
            "tmp/reports/platform-host-evidence/linux-x64/package/"
            "runtime-library-manifest.json"
        ),
        "reviewed_source_required": True,
        "support_truth": False,
        "generated_report_support_truth": False,
        "native_execution_claimed": False,
        "promotion_allowed_from_generated_evidence": False,
        "review_result": "fail-closed-not-promotion-ready",
        "status": "missing-source-generated-fail-closed",
        "source_artifacts": [
            {
                "path": "artifacts/package/objc3c-runnable-toolchain-package.json",
                "exists": False,
            }
        ],
        "diagnostics": {
            "status": "missing-source-generated-fail-closed",
            "classification": "incomplete-review-candidate",
            "review_result": "fail-closed-not-promotion-ready",
            "required_source_artifacts": [
                "artifacts/package/objc3c-runnable-toolchain-package.json"
            ],
        },
    }
    output_path = (
        tmp_path
        / "tmp/reports/platform-host-evidence/linux-x64/package/runtime-library-manifest.json"
    )
    output_path.parent.mkdir(parents=True)
    host_evidence_ingest.write_json(output_path, payload)

    host_evidence_ingest.validate_runtime_library_manifest_artifact("linux-x64")


def test_runtime_manifest_incomplete_source_artifacts_cover_runtime_library(
    monkeypatch: pytest.MonkeyPatch,
    tmp_path,
) -> None:
    monkeypatch.setattr(host_evidence_ingest, "ROOT", tmp_path)
    build_summary_path = tmp_path / "tmp/build-objc3c-native/native_build_summary.json"
    build_summary_path.parent.mkdir(parents=True)
    host_evidence_ingest.write_json(build_summary_path, {"target": {}})
    runtime_path = tmp_path / "artifacts/lib/libobjc3-runtime.so"
    runtime_path.parent.mkdir(parents=True)
    runtime_path.write_bytes(b"not a real elf; provenance only")

    host_evidence_ingest.write_runtime_library_manifest_artifact("linux-x64")

    payload = host_evidence_ingest.load_platform_generated_json(
        "linux-x64",
        "package/runtime-library-manifest.json",
    )
    source_paths = [
        entry["path"]
        for entry in payload["source_artifacts"]
    ]
    assert "artifacts/lib/libobjc3-runtime.so" in source_paths
    assert (
        "artifacts/lib/libobjc3-runtime.so"
        in payload["diagnostics"]["required_source_artifacts"]
    )
    host_evidence_ingest.validate_runtime_library_manifest_artifact("linux-x64")


def test_powershell_linux_evidence_source_artifact_lists_are_arrays(tmp_path) -> None:
    pwsh = shutil.which("pwsh") or shutil.which("powershell")
    if pwsh is None:
        pytest.skip("PowerShell is not available")
    repo_root = Path.cwd()
    evidence_root = tmp_path / "linux-evidence"
    script = f"""
$ErrorActionPreference = 'Stop'
Import-Module '{repo_root / "scripts" / "objc3c_platform_host_evidence_producers.psm1"}' -Force -DisableNameChecking
$repoRoot = '{repo_root}'
$readme = Join-Path $repoRoot 'README.md'
Write-Objc3cLinuxObjectDebugIdentityEvidence `
  -RepoRoot $repoRoot `
  -EvidenceRoot '{evidence_root}' `
  -PlatformId 'linux-x64' `
  -TargetTriple 'x86_64-unknown-linux-gnu' `
  -ObjectFormat 'ELF' `
  -DebugFormat 'DWARF' `
  -NativeExecutablePath $readme `
  -CapiRunnerPath $readme `
  -RuntimeLibraryPath $readme `
  -BuildSummaryPath $readme
"""
    result = subprocess.run(
        [pwsh, "-NoLogo", "-NoProfile", "-Command", script],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr
    assert (evidence_root / "build/object-identity.json").is_file()
    assert (evidence_root / "build/debug-identity.json").is_file()
