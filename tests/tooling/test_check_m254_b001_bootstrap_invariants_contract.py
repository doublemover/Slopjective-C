from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m254_b001_bootstrap_invariants_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m254_b001_bootstrap_invariants_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m254_b001_bootstrap_invariants_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _native_is_stale() -> bool:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        return True
    native_mtime = contract.DEFAULT_NATIVE_EXE.stat().st_mtime
    freshness_inputs = (
        contract.DEFAULT_AST_HEADER,
        contract.DEFAULT_FRONTEND_TYPES,
        contract.DEFAULT_FRONTEND_ARTIFACTS,
        contract.DEFAULT_DRIVER_CPP,
        contract.DEFAULT_PROCESS_CPP,
    )
    return any(path.stat().st_mtime > native_mtime for path in freshness_inputs)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m254-b001-bootstrap-invariants-contract-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["next_implementation_issue"] == "M254-B002"
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m254_b001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m254/M254-B001/")


def test_contract_fails_closed_when_expectations_drop_duplicate_registration_policy(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m254_bootstrap_invariants_contract_and_architecture_freeze_b001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`fail-closed-by-translation-unit-identity-key`",
            "`duplicate-registration-policy-v2`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--expectations-doc",
            str(drift_doc),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M254-B001-DOC-EXP-04"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_frontend_artifacts_drop_bootstrap_surface_name(tmp_path: Path) -> None:
    drift_artifacts = tmp_path / "objc3_frontend_artifacts.cpp"
    drift_artifacts.write_text(
        contract.DEFAULT_FRONTEND_ARTIFACTS.read_text(encoding="utf-8").replace(
            "objc_runtime_startup_bootstrap_invariants",
            "objc_runtime_startup_bootstrap_surface",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--frontend-artifacts",
            str(drift_artifacts),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M254-B001-ART-03"
        for failure in payload["failures"]
    )


def test_dynamic_probe_covers_bootstrap_invariant_packet(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")
    if _native_is_stale():
        pytest.skip("native frontend is older than the B001 source inputs")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 1

    case = payload["dynamic_cases"][0]
    assert case["fixture"] == "tests/tooling/fixtures/native/hello.objc3"
    assert case["manifest_path"].endswith("module.manifest.json")
    assert case["registration_manifest_path"].endswith(
        "module.runtime-registration-manifest.json"
    )
    assert (
        case["duplicate_registration_policy"]
        == "fail-closed-by-translation-unit-identity-key"
    )
    assert (
        case["realization_order_policy"]
        == "constructor-root-then-registration-manifest-order"
    )
    assert case["bootstrap_replay_key"]
