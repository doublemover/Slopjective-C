from __future__ import annotations

import importlib.util
import subprocess
import sys
from pathlib import Path
from types import ModuleType

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_shared.json_io import load_json_object


SCRIPT_PATH = ROOT / "scripts" / "check_objc3c_public_conformance_suite.py"


def load_checker() -> ModuleType:
    spec = importlib.util.spec_from_file_location("check_objc3c_public_conformance_suite", SCRIPT_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError("Unable to load public conformance suite package checker")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def test_public_conformance_suite_package_checker_stages_replayable_package(tmp_path: Path) -> None:
    checker = load_checker()
    package_root = ROOT / "tmp" / "tests" / "objc3-public-conformance-suite-package"
    report_path = ROOT / "tmp" / "tests" / "public-suite-package-summary.json"

    assert checker.main(["--package-root", str(package_root), "--report-path", str(report_path)]) == 0

    summary = load_json_object(report_path)
    package_manifest = load_json_object(package_root / "package-manifest.json")

    assert summary["contract_id"] == "objc3c.public_conformance_suite.package_replay.summary.v1"
    assert summary["status"] == "PASS"
    assert summary["case_count"] == 11
    assert summary["verified_case_count"] == 11
    assert summary["source_file_count"] == summary["verified_source_file_count"]
    assert summary["offline_compatible"] is True
    assert summary["tmp_source_truth_allowed"] is False
    assert summary["generated_reports_are_evidence_only"] is True
    assert summary["source_owned_contract_count"] == 3
    assert summary["package_manifest_replay_action_count"] == 4
    assert summary["package_manifest_source_hash_algorithm"] == "sha256"
    assert summary["generated_output_roots"] == [
        "tmp/reports/conformance",
        "tmp/artifacts/public-conformance/suite",
        "tmp/pkg/objc3-public-conformance-suite",
    ]
    assert "npm run objc3c -- validate-release-candidate-conformance" in summary["public_commands"]
    assert "npm run objc3c -- validate-cross-lane-e2e" in summary["public_commands"]

    assert package_manifest["contract_id"] == "objc3c.public_conformance_suite.package_manifest.v1"
    assert load_json_object(package_root / "package.json")["scripts"]["objc3c"] == (
        "python tools/replay_public_conformance_suite.py"
    )
    assert (package_root / "tools" / "replay_public_conformance_suite.py").is_file()
    assert package_manifest["source_truth_policy"] == {
        "checked_in_source_truth_required": True,
        "tmp_source_truth_allowed": False,
        "generated_reports_are_evidence_only": True,
    }
    assert package_manifest["artifact_contract"]["issue_id"] == "OBJ3-NEXT-018"
    assert package_manifest["replay_requirements"]["package_manifest_contract"] == (
        "objc3c.public_conformance_suite.package_manifest.v1"
    )
    assert package_manifest["replay_requirements"]["source_hash_algorithm"] == "sha256"
    assert {
        action["action"]: tuple(action["profile_ids"])
        for action in package_manifest["replay_requirements"]["required_actions"]
    } == {
        "validate-conformance-corpus": ("core",),
        "validate-interop-conformance": ("stdlib-package",),
        "validate-release-candidate-conformance": ("release-candidate",),
        "validate-public-conformance-suite": ("core", "stdlib-package", "release-candidate"),
    }
    assert package_manifest["artifact_contract"]["package_replay_boundary"]["public_commands_only"] is True
    assert package_manifest["case_count"] == len(package_manifest["cases"]) == 11
    assert all(case["release_gate"] is True for case in package_manifest["cases"])
    assert [case["stable_case_index"] for case in package_manifest["cases"]] == list(range(1, 12))
    assert all(
        source["package_path"].startswith("sources/")
        and not source["repo_path"].startswith("tmp/")
        and (package_root / source["package_path"]).is_file()
        for source in package_manifest["source_files"]
    )

    case_path = package_root / package_manifest["cases"][0]["case_manifest"]
    case_manifest = load_json_object(case_path)
    assert case_manifest["contract_id"] == "objc3c.public_conformance_suite.case.v1"
    assert case_manifest["stable_case_index"] == package_manifest["cases"][0]["stable_case_index"]
    assert case_manifest["fixture_provenance"] == {
        "origin": "checked-in-public-suite",
        "owner": "objc3-public-conformance",
        "source_owned": True,
        "internal_only": False,
        "generated": False,
    }
    assert case_manifest["positive_evidence"]
    assert case_manifest["negative_evidence"]
    assert case_manifest["runnable_command"].startswith("npm run objc3c -- ")
    assert all((package_root / source["package_path"]).is_file() for source in case_manifest["packaged_source_files"])

    completed = subprocess.run(
        [
            sys.executable,
            str(package_root / "tools" / "replay_public_conformance_suite.py"),
            "validate-release-candidate-conformance",
        ],
        cwd=package_root,
        check=False,
        capture_output=True,
        text=True,
    )
    assert completed.returncode == 0, completed.stderr
    replay_summary = load_json_object(
        package_root / "tmp" / "reports" / "conformance" / "validate-release-candidate-conformance.json"
    )
    assert replay_summary["contract_id"] == "objc3c.public_conformance_suite.packaged_replay.v1"
    assert replay_summary["status"] == "PASS"
    assert replay_summary["case_count"] == 11
    assert replay_summary["generated_reports_are_evidence_only"] is True

    completed = subprocess.run(
        [
            sys.executable,
            str(package_root / "tools" / "replay_public_conformance_suite.py"),
            "validate-public-conformance-suite",
        ],
        cwd=package_root,
        check=False,
        capture_output=True,
        text=True,
    )
    assert completed.returncode == 0, completed.stderr
    replay_summary = load_json_object(
        package_root / "tmp" / "reports" / "conformance" / "validate-public-conformance-suite.json"
    )
    assert replay_summary["contract_id"] == "objc3c.public_conformance_suite.packaged_replay.v1"
    assert replay_summary["case_count"] == 11


def test_public_conformance_suite_package_contract_is_checked_source_truth() -> None:
    contract = load_json_object(
        ROOT / "tests" / "tooling" / "fixtures" / "public_conformance_suite" / "package_contract.json"
    )

    assert contract["contract_id"] == "objc3c.public_conformance_suite.package_contract.v1"
    assert contract["source_manifest"] == "tests/conformance/public_suite_manifest.json"
    assert contract["package_replay_evidence"] == "tests/conformance/public_suite_package_replay_evidence.json"
    assert set(contract["source_owned_contracts"]) == {
        "tests/conformance/public_suite_manifest.json",
        "tests/conformance/public_suite_package_replay_evidence.json",
        "tests/tooling/fixtures/public_conformance_suite/package_contract.json",
    }
    assert contract["generated_output_boundary"] == {
        "required_tmp_roots": [
            "tmp/reports/conformance",
            "tmp/artifacts/public-conformance/suite",
            "tmp/pkg/objc3-public-conformance-suite",
        ],
        "generated_outputs_committable": False,
        "generated_outputs_can_define_support": False,
    }
    assert contract["required_package_outputs"] == [
        "package-manifest.json",
        "package.json",
        "replay-plan.json",
        "README.md",
        "tools/replay_public_conformance_suite.py",
        "cases",
    ]
    assert "tmp artifacts are never source truth" in contract["fail_closed_invariants"]
    assert "unsupported claims cannot be promoted by packaged replay" in contract["fail_closed_invariants"]
    assert "every public-stable case carries source-owned fixture provenance" in contract["fail_closed_invariants"]


def test_public_conformance_suite_support_row_cites_package_replay_source_truth() -> None:
    matrix = load_json_object(ROOT / "docs" / "support" / "capability_matrix.json")
    rows = {row["id"]: row for row in matrix["capabilities"]}  # type: ignore[index]
    row = rows["conformance.public.stable-suite-manifest"]
    evidence_paths = {item["path"] for item in row["evidence"]}
    evidence_commands = {
        item["command"]
        for item in row["evidence"]
        if "command" in item
    }

    assert row["state"] == "implemented"
    assert row["support_claims"] == [
        "objc3c.behavior.conformance.public-stable-suite"
    ]
    assert "tests/conformance/public_suite_package_replay_evidence.json" in evidence_paths
    assert "tests/tooling/fixtures/public_conformance_suite/package_contract.json" in evidence_paths
    assert "scripts/objc3c_public_conformance_suite/package.py" in evidence_paths
    assert "npm run objc3c -- validate-public-conformance-suite" in evidence_commands


def test_public_workflow_action_uses_package_checker() -> None:
    catalog = load_json_object(ROOT / "scripts" / "objc3c_workflow" / "schemas" / "action-registry-v1.schema.json")
    assert catalog["$schema"] == "https://json-schema.org/draft/2020-12/schema"

    from scripts.objc3c_workflow.action_catalog_conformance import CONFORMANCE_ACTION_SPECS
    from scripts.objc3c_workflow.actions.application_surface_paths import PUBLIC_CONFORMANCE_SUITE_PY

    action = CONFORMANCE_ACTION_SPECS["validate-public-conformance-suite"]
    assert action.backend == "python:scripts/check_objc3c_public_conformance_suite.py"
    assert PUBLIC_CONFORMANCE_SUITE_PY == ROOT / "scripts" / "check_objc3c_public_conformance_suite.py"
