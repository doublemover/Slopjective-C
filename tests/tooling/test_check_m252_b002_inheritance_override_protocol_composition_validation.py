from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m252_b002_inheritance_override_protocol_composition_validation.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m252_b002_inheritance_override_protocol_composition_validation",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m252_b002_inheritance_override_protocol_composition_validation.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-runner-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m252-b002-inheritance-override-protocol-composition-validation-v1"
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is False
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m252_b002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m252/M252-B002/")


def test_contract_fails_closed_when_expectations_drop_override_edge_anchor(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m252_inheritance_override_protocol_composition_validation_b002_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`method-to-overridden-method`",
            "`method override edges`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([
        "--expectations-doc",
        str(drift_doc),
        "--skip-runner-probes",
        "--summary-out",
        str(summary_out),
    ])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-B002-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_pipeline_drops_override_edge_materialization(tmp_path: Path) -> None:
    drift_pipeline = tmp_path / "objc3_frontend_pipeline.cpp"
    drift_pipeline.write_text(
        contract.DEFAULT_FRONTEND_PIPELINE.read_text(encoding="utf-8").replace(
            'add_owner_edge("method-to-overridden-method", method_node.owner_identity,',
            'add_owner_edge("method-to-base-method", method_node.owner_identity,',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([
        "--frontend-pipeline",
        str(drift_pipeline),
        "--skip-runner-probes",
        "--summary-out",
        str(summary_out),
    ])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-B002-PIPE-01" for failure in payload["failures"])


def test_dynamic_runner_probe_accepts_happy_path_fixture(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists():
        pytest.skip("native frontend C API runner binary is not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is True

    runner_case = payload["runner_case"]
    surface = runner_case["surface"]
    assert surface["ready"] is True
    assert surface["inheritance_validation_ready"] is True
    assert surface["override_validation_ready"] is True
    assert surface["protocol_composition_validation_ready"] is True
    assert surface["metaclass_relationship_validation_ready"] is True
    assert surface["override_edge_count"] == 2
