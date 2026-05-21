from __future__ import annotations

import importlib.util
import sys
from pathlib import Path
from types import ModuleType
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object, write_json_file

SCRIPT_PATH = ROOT / "scripts" / "check_objc3c_public_conformance_suite_manifest.py"


def load_checker() -> ModuleType:
    spec = importlib.util.spec_from_file_location(
        "check_objc3c_public_conformance_suite_manifest",
        SCRIPT_PATH,
    )
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load public suite checker")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def load_manifest() -> dict[str, Any]:
    return load_json_object(ROOT / "tests" / "conformance" / "public_suite_manifest.json")


def write_manifest(path: Path, manifest: dict[str, Any]) -> None:
    write_json_file(path, manifest, sort_keys=False)


def case_by_id(manifest: dict[str, Any], case_id: str) -> dict[str, Any]:
    for suite_case in manifest["suite_cases"]:
        if suite_case["case_id"] == case_id:
            return suite_case
    raise AssertionError(f"missing suite case: {case_id}")


def test_public_conformance_suite_manifest_passes_and_reports_public_taxonomy() -> None:
    checker = load_checker()
    checker.SUMMARY_PATH = ROOT / "tmp" / "tests" / "public-suite-summary.json"
    checker.SUMMARY_PATH.unlink(missing_ok=True)

    try:
        assert checker.main() == 0
        summary = load_json_object(checker.SUMMARY_PATH)
    finally:
        checker.SUMMARY_PATH.unlink(missing_ok=True)

    assert summary["contract_id"] == "objc3c.public_conformance_suite.summary.v1"
    assert summary["status"] == "PASS"
    assert summary["case_count"] == 10
    assert summary["phase_count"] == 8
    assert summary["profile_count"] == 3
    assert summary["packageable"] is True
    assert summary["artifact_contract_issue_id"] == "OBJ3-NEXT-018"
    assert summary["source_owned_contract_count"] == 3
    assert summary["package_manifest_replay_contract"] == "objc3c.public_conformance_suite.package_manifest.v1"
    assert summary["package_manifest_replay_action_count"] == 4
    assert summary["package_manifest_required_file_count"] == 6
    assert summary["package_manifest_source_hash_algorithm"] == "sha256"
    assert summary["source_owned_case_count"] == 10
    assert summary["fixture_provenance_allowed_origins"] == ["checked-in-public-suite"]
    assert summary["fixture_provenance_origin_counts"] == {"checked-in-public-suite": 10}
    assert summary["allowed_generated_roots"] == [
        "tmp/reports/conformance",
        "tmp/artifacts/public-conformance/suite",
        "tmp/pkg/objc3-public-conformance-suite",
    ]
    assert summary["outside_repo_replay_required"] is True
    assert summary["public_commands_only"] is True
    assert summary["fixture_boundary"] == {
        "public_fixture_root_count": 7,
        "internal_only_root_count": 3,
        "checked_in_expected_outputs_required": True,
    }
    assert summary["accepted_external_validation_entries"] == 3
    assert summary["rejected_external_validation_entries"] == 3
    assert summary["release_candidate_public_stable_case_count"] == 10
    assert summary["release_candidate_required_phase_count"] == 8
    assert summary["release_candidate_gate_command"] == "npm run objc3c -- validate-release-candidate-conformance"
    assert summary["phase_case_counts"]["release_candidate"] == 2
    assert summary["phase_case_counts"]["sema"] == 2
    assert "npm run objc3c -- validate-release-candidate-conformance" in summary["public_commands"]
    assert "npm run objc3c -- validate-conformance-corpus" in summary["public_commands"]
    assert "objc3c.behavior.parser.canonical-syntax" in summary["support_claims"]
    assert "objc3c.behavior.conformance.public-stable-suite" in summary["support_claims"]
    assert "objc3c.behavior.diagnostics.parser-sema-recovery-fixits" in summary["support_claims"]


def test_public_conformance_suite_manifest_cites_packaged_outside_repo_replay_evidence() -> None:
    manifest = load_manifest()
    replay_case = case_by_id(
        manifest,
        "public.conformance.public-stable-suite-package-replay",
    )
    evidence_path = ROOT / "tests" / "conformance" / "public_suite_package_replay_evidence.json"
    evidence = load_json_object(evidence_path)

    assert replay_case["conformance_fixture"] == "tests/conformance/public_suite_package_replay_evidence.json"
    assert replay_case["stable_case_index"] == 9
    assert replay_case["fixture_provenance"] == {
        "origin": "checked-in-public-suite",
        "owner": "objc3-public-conformance",
        "source_owned": True,
        "internal_only": False,
        "generated": False,
    }
    assert "tests/conformance/public_suite_package_replay_evidence.json" in replay_case["positive_evidence"]
    assert replay_case["support_claim"] == "objc3c.behavior.conformance.public-stable-suite"
    assert replay_case["release_gate"] is True

    assert evidence["contract_id"] == "objc3c.public_conformance_suite.package_replay_evidence.v1"
    assert evidence["outside_repo_replay"] == {
        "required": True,
        "model": "copy staged suite package into a clean directory outside the repository root and run only packaged public commands",
        "source_truth_policy": "checked-in manifests and fixtures define support; generated package and report trees prove replay only",
        "network_policy": "offline",
        "tmp_support_claims_allowed": False,
    }
    assert all(
        entry["command"].startswith("npm run objc3c -- ")
        and entry["requires_repo_checkout"] is False
        and entry["requires_network"] is False
        for entry in evidence["packaged_entrypoints"]
    )
    assert {
        "core",
        "stdlib-package",
        "release-candidate",
    } == {entry["profile_id"] for entry in evidence["packaged_entrypoints"]}
    replay_outputs = evidence["required_replay_outputs"]
    assert "tmp/pkg/objc3-public-conformance-suite/package-manifest.json" in replay_outputs
    assert evidence["source_manifest"] == "tests/conformance/public_suite_manifest.json"
    assert evidence["package_manifest_replay"]["package_manifest_contract"] == (
        "objc3c.public_conformance_suite.package_manifest.v1"
    )
    assert evidence["package_manifest_replay"]["package_json_script"] == (
        "python tools/replay_public_conformance_suite.py"
    )
    assert evidence["package_manifest_replay"]["source_hash_algorithm"] == "sha256"
    assert {
        entry["action"]: tuple(entry["profile_ids"])
        for entry in evidence["package_manifest_replay"]["required_actions"]
    } == {
        "validate-conformance-corpus": ("core",),
        "validate-interop-conformance": ("stdlib-package",),
        "validate-release-candidate-conformance": ("release-candidate",),
        "validate-public-conformance-suite": ("core", "stdlib-package", "release-candidate"),
    }


def test_public_conformance_suite_manifest_declares_external_validation_intake_policy() -> None:
    manifest = load_manifest()
    policy = manifest["external_validation_policy"]
    release_profile = manifest["release_candidate_profile"]

    assert policy["source_surface"] == "tests/tooling/fixtures/external_validation/source_surface.json"
    assert policy["trust_policy"] == "tests/tooling/fixtures/external_validation/trust_policy.json"
    assert policy["intake_manifest"] == "tests/tooling/fixtures/external_validation/intake_manifest.json"
    assert policy["quarantine_manifest"] == "tests/tooling/fixtures/external_validation/quarantine_manifest.json"
    assert policy["support_claim_gate"] == "tests/tooling/fixtures/external_validation/support_claim_gate.json"
    assert policy["admitted_trust_states"] == ["accepted"]
    assert set(policy["rejected_trust_states"]) == {"candidate", "quarantined", "rejected"}
    assert policy["external_evidence_can_create_public_support_claim"] is False
    assert policy["tmp_artifact_support_allowed"] is False
    assert "cannot override capability matrix" in policy["capability_truth_policy"]

    assert release_profile["profile_id"] == "release-candidate"
    assert release_profile["requires_all_public_stable_cases"] is True
    assert release_profile["consumes_external_validation_policy"] is True
    assert release_profile["tmp_artifact_support_allowed"] is False
    assert release_profile["package_replay_case_id"] == "public.conformance.public-stable-suite-package-replay"


def test_public_conformance_suite_manifest_declares_source_owned_artifact_contract() -> None:
    manifest = load_manifest()
    artifact_contract = manifest["artifact_contract"]

    assert artifact_contract["issue_id"] == "OBJ3-NEXT-018"
    assert artifact_contract["metadata_policy"]["stable_case_indices_required"] is True
    assert artifact_contract["metadata_policy"]["deterministic_case_order"] == "stable_case_index"
    assert artifact_contract["metadata_policy"]["deterministic_phase_order"] == [
        "parser",
        "sema",
        "lowering",
        "ir",
        "runtime",
        "stdlib",
        "package",
        "release_candidate",
    ]
    assert {
        entry["role"]: (entry["path"], entry["source_owned"])
        for entry in artifact_contract["source_owned_contracts"]
    } == {
        "suite-manifest": ("tests/conformance/public_suite_manifest.json", True),
        "package-replay-evidence": ("tests/conformance/public_suite_package_replay_evidence.json", True),
        "package-contract": ("tests/tooling/fixtures/public_conformance_suite/package_contract.json", True),
    }
    assert artifact_contract["generated_output_policy"]["generated_outputs_committable"] is False
    assert artifact_contract["generated_output_policy"]["generated_outputs_can_define_support"] is False
    assert artifact_contract["package_replay_boundary"] == {
        "outside_repo_replay_required": True,
        "offline_required": True,
        "repo_checkout_required": False,
        "public_commands_only": True,
        "unsupported_fixture_packaging_allowed": False,
    }


def test_public_conformance_suite_manifest_rejects_compatibility_mode(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["strict_rejection_policy"]["compatibility_mode_allowed"] = True

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_external_validation_claim_creation(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["external_validation_policy"]["external_evidence_can_create_public_support_claim"] = True

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_internal_only_public_case_source(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["suite_cases"][0]["positive_evidence"] = [
        "tests/conformance/spec_open_issues/README.md"
    ]

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_tmp_public_case_source(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["suite_cases"][0]["positive_evidence"] = [
        "tmp/reports/conformance/leaked-public-claim.json"
    ]

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_generated_fixture_provenance(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["suite_cases"][0]["fixture_provenance"]["generated"] = True

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_unstable_case_index(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["suite_cases"][1]["stable_case_index"] = 99

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_generated_boundary_escape(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["artifact_contract"]["generated_output_policy"]["allowed_generated_roots"] = [
        "docs/support"
    ]

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_release_candidate_subset(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    case = case_by_id(manifest, "public.stdlib.core-runtime-backed-v1")
    case["profile_ids"] = ["stdlib-package"]

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_missing_capability_pair(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["suite_cases"][0]["support_claim"] = "objc3c.behavior.parser.unowned-public-claim"

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_nonpublic_command(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    manifest = load_manifest()
    manifest["suite_cases"][0]["runnable_command"] = "python scripts/private_runner.py"

    checker.MANIFEST_PATH = tmp_path / "public_suite_manifest.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_manifest(checker.MANIFEST_PATH, manifest)

    assert checker.main() == 1
    assert not checker.SUMMARY_PATH.exists()


def test_public_conformance_suite_manifest_rejects_missing_package_replay_action(
    tmp_path: Path,
) -> None:
    checker = load_checker()
    replay_evidence = load_json_object(ROOT / "tests" / "conformance" / "public_suite_package_replay_evidence.json")
    replay_evidence["package_manifest_replay"]["required_actions"] = [
        action
        for action in replay_evidence["package_manifest_replay"]["required_actions"]
        if action["action"] != "validate-public-conformance-suite"
    ]

    checker.PACKAGE_REPLAY_EVIDENCE_PATH = tmp_path / "public_suite_package_replay_evidence.json"
    checker.SUMMARY_PATH = tmp_path / "summary.json"
    write_json_file(checker.PACKAGE_REPLAY_EVIDENCE_PATH, replay_evidence, sort_keys=False)

    try:
        assert checker.main() == 1
        assert not checker.SUMMARY_PATH.exists()
    finally:
        checker.PACKAGE_REPLAY_EVIDENCE_PATH = ROOT / "tests" / "conformance" / "public_suite_package_replay_evidence.json"
