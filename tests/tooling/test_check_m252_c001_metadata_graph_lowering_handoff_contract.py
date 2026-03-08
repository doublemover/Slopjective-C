from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m252_c001_metadata_graph_lowering_handoff_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m252_c001_metadata_graph_lowering_handoff_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m252_c001_metadata_graph_lowering_handoff_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-runner-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m252-c001-metadata-graph-lowering-handoff-contract-v1"
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is False
    assert payload["checks_total"] >= 35
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m252_c001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m252/M252-C001/")


def test_contract_fails_closed_when_expectations_drop_contract_id_anchor(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m252_metadata_graph_lowering_handoff_contract_and_architecture_freeze_c001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "objc3c-executable-metadata-lowering-handoff-freeze/m252-c001-v1",
            "objc3c-metadata-handoff/m252-c001-v1",
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
    assert any(failure["check_id"] == "M252-C001-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_frontend_types_drop_handoff_surface_anchor(tmp_path: Path) -> None:
    drift_types = tmp_path / "objc3_frontend_types.h"
    drift_types.write_text(
        contract.DEFAULT_FRONTEND_TYPES.read_text(encoding="utf-8").replace(
            "struct Objc3ExecutableMetadataLoweringHandoffSurface",
            "struct Objc3ExecutableMetadataLoweringBoundary",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([
        "--frontend-types",
        str(drift_types),
        "--skip-runner-probes",
        "--summary-out",
        str(summary_out),
    ])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M252-C001-TYPES-02" for failure in payload["failures"])


def test_dynamic_runner_probes_cover_class_and_category_metadata_handoff_cases(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists():
        pytest.skip("native frontend C API runner binary is not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["runner_probes_executed"] is True

    runner_cases = payload["runner_cases"]
    assert runner_cases["class_protocol_property_ivar"]["handoff_ready"] is True
    assert runner_cases["class_protocol_property_ivar"]["parse_handoff_key_matches"] is True
    assert runner_cases["category_protocol_property"]["handoff_ready"] is True
    assert runner_cases["category_protocol_property"]["parse_handoff_key_matches"] is True
