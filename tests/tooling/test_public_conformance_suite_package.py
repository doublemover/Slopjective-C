from __future__ import annotations

import importlib.util
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


def test_public_conformance_package_refuses_to_stage_missing_prototype_inputs(
    tmp_path: Path, capsys,
) -> None:
    checker = load_checker()
    package_root = tmp_path / "package"
    report_path = tmp_path / "summary.json"

    assert checker.main(["--package-root", str(package_root), "--report-path", str(report_path)]) == 1
    assert "public conformance suite manifest checker failed" in capsys.readouterr().err
    assert not (package_root / "package-manifest.json").exists()
    assert not report_path.exists()


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
