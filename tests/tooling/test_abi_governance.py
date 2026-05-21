from __future__ import annotations

import copy
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

import check_objc3c_abi_governance as abi_governance
from objc3c_shared.json_io import load_json_object, write_json_file
from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS
from scripts.objc3c_workflow.public_command_api import (
    public_workflow_action_names,
    public_workflow_action_payload,
)

MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "abi_governance" / "source_of_truth_manifest.json"
RELEASE_GOVERNANCE = (
    ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "abi_api_governance.json"
)


def _manifest() -> dict[str, object]:
    return load_json_object(MANIFEST)


def _run_manifest(tmp_path: Path, manifest: dict[str, object]) -> tuple[int, dict[str, object]]:
    manifest_path = tmp_path / "source_of_truth_manifest.json"
    summary_path = tmp_path / "abi-governance-summary.json"
    write_json_file(manifest_path, manifest, sort_keys=True)
    return abi_governance.run_check(manifest_path=manifest_path, summary_path=summary_path)


def test_abi_governance_accepts_checked_in_manifest(tmp_path: Path) -> None:
    rc, summary = abi_governance.run_check(
        manifest_path=MANIFEST,
        summary_path=tmp_path / "summary.json",
    )

    assert rc == 0
    assert summary["status"] == "PASS"
    assert summary["diagnostic_code"] == "O3ABI8173"
    assert summary["governed_surface_count"] == 3
    assert summary["surface_extractor_count"] == 3
    observed = {
        item["extractor_id"]: item for item in summary["surface_extractors"]  # type: ignore[index]
    }
    assert observed["runtime-public-c-header-symbols"]["observed_count"] == 122
    assert observed["stdlib-module-abi-signatures"]["observed_count"] == 147
    assert observed["package-lock-abi-identity-schema"]["observed_count"] == 3
    assert summary["release_blocker_issue_refs"] == ["#8173"]


def test_abi_governance_public_workflow_action_is_registered() -> None:
    action = "validate-abi-governance"
    payload = public_workflow_action_payload(action)

    assert action in public_workflow_action_names()
    assert action in ACTION_HANDLERS
    assert payload["backend"] == "python:scripts/check_objc3c_abi_governance.py"
    assert payload["validation_tier"] == "repo"
    assert "source-owned" in str(payload["guarantee_owner"])


def test_abi_governance_support_row_is_source_owned_and_non_release_channel() -> None:
    matrix = load_json_object(ROOT / "docs" / "support" / "capability_matrix.json")
    rows = {row["id"]: row for row in matrix["capabilities"]}  # type: ignore[index]
    row = rows["abi.governance.source-truth"]
    evidence_paths = {item["path"] for item in row["evidence"]}
    evidence_commands = {
        item["command"]
        for item in row["evidence"]
        if "command" in item
    }

    assert row["state"] == "implemented"
    assert row["support_claims"] == [
        "objc3c.behavior.abi.governance-source-truth"
    ]
    assert "does not perform release channel publication" in row["summary"]
    assert "scripts/check_objc3c_abi_governance.py" in evidence_paths
    assert "tests/tooling/fixtures/abi_governance/source_of_truth_manifest.json" in evidence_paths
    assert "npm run objc3c -- validate-abi-governance" in evidence_commands


def test_abi_governance_rejects_missing_issue_8173(tmp_path: Path) -> None:
    manifest = _manifest()
    manifest["issue_refs"] = ["#8100"]

    rc, summary = _run_manifest(tmp_path, manifest)

    assert rc == 1
    assert any(
        "O3ABI8173" in failure and "schema validation failed" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_abi_governance_rejects_compatibility_shim_claim(tmp_path: Path) -> None:
    manifest = _manifest()
    policy = manifest["compatibility_policy"]  # type: ignore[index]
    policy["shim_policy"] = "compatibility-shims-supported"

    rc, summary = _run_manifest(tmp_path, manifest)

    assert rc == 1
    assert any(
        "O3ABI8173" in failure
        and (
            "schema validation failed" in failure
            or "must reject compatibility shims" in failure
        )
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_abi_governance_rejects_governed_surface_identity_drift(tmp_path: Path) -> None:
    manifest = _manifest()
    surfaces = manifest["governed_surfaces"]  # type: ignore[index]
    surfaces[1]["expected_identity"] = "objc3-abi-drifted"

    rc, summary = _run_manifest(tmp_path, manifest)

    assert rc == 1
    assert any(
        "governed surface abi-artifact-manifest-schema identity drifted" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_abi_governance_rejects_temp_source_of_truth_paths(tmp_path: Path) -> None:
    manifest = _manifest()
    surfaces = manifest["governed_surfaces"]  # type: ignore[index]
    surfaces[0]["source_path"] = "tmp/reports/abi-governance/generated.json"

    rc, summary = _run_manifest(tmp_path, manifest)

    assert rc == 1
    assert any(
        "must not point at generated temp output" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_abi_governance_rejects_extracted_public_header_digest_drift(
    tmp_path: Path,
) -> None:
    manifest = _manifest()
    extractors = manifest["surface_extractors"]  # type: ignore[index]
    extractors[0]["expected_digest"] = (
        "sha256:1111111111111111111111111111111111111111111111111111111111111111"
    )

    rc, summary = _run_manifest(tmp_path, manifest)

    assert rc == 1
    assert any(
        "surface extractor runtime-public-c-header-symbols digest drifted" in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_abi_governance_rejects_generated_extraction_sources(tmp_path: Path) -> None:
    manifest = _manifest()
    extractors = manifest["surface_extractors"]  # type: ignore[index]
    extractors[1]["source_globs"] = ["tmp/artifacts/generated-abi/*.json"]

    rc, summary = _run_manifest(tmp_path, manifest)

    assert rc == 1
    assert any(
        "extractor stdlib-module-abi-signatures.source_globs must not point at generated temp output"
        in failure
        for failure in summary["failures"]  # type: ignore[index]
    )


def test_abi_governance_rejects_release_governance_lost_blocked_transition(
    tmp_path: Path,
) -> None:
    release_manifest = load_json_object(RELEASE_GOVERNANCE)
    release_copy = copy.deepcopy(release_manifest)
    blocked = release_copy["allowed_transition_policy"]["release_blocked_transitions"]  # type: ignore[index]
    blocked.remove("unsupported-downgrade-route")
    release_copy_path = tmp_path / "release_governance.json"
    write_json_file(release_copy_path, release_copy, sort_keys=True)

    rc, summary = abi_governance.run_check(
        manifest_path=MANIFEST,
        summary_path=tmp_path / "summary.json",
        release_governance_override=release_copy_path,
    )

    assert rc == 1
    assert any(
        "release ABI/API governance lost release blockers: unsupported-downgrade-route"
        in failure
        for failure in summary["failures"]  # type: ignore[index]
    )
