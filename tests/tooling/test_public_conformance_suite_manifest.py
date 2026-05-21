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
    assert "tmp/pkg/objc3-public-conformance-suite/package-manifest.json" in evidence["required_replay_outputs"]
    assert evidence["source_manifest"] == "tests/conformance/public_suite_manifest.json"


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
