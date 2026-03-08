from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m253_d001_object_packaging_and_retention_contract_and_architecture_freeze.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m253_d001_object_packaging_and_retention_contract_and_architecture_freeze",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m253_d001_object_packaging_and_retention_contract_and_architecture_freeze.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _npm_cmd() -> str:
    return "npm.cmd" if sys.platform == "win32" else "npm"


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m253-d001-object-packaging-and-retention-contract-and-architecture-freeze-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m253_d001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m253/M253-D001/")


def test_contract_fails_closed_when_expectations_drop_boundary_model(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            f"`{contract.BOUNDARY_MODEL}`",
            "`changed-boundary-model`",
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
    assert any(failure["check_id"] == "M253-D001-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_ir_emitter_drops_object_packaging_named_metadata(tmp_path: Path) -> None:
    drift_ir = tmp_path / "objc3_ir_emitter.cpp"
    drift_ir.write_text(
        contract.DEFAULT_IR_EMITTER.read_text(encoding="utf-8").replace(
            'out << "!objc3.objc_runtime_object_packaging_retention = !{!61}\\n";',
            'out << "!objc3.objc_runtime_object_packaging = !{!61}\\n";',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--ir-emitter",
            str(drift_ir),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M253-D001-IR-01" for failure in payload["failures"])


def test_dynamic_probe_records_object_packaging_retention_boundary(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binary is not built")

    subprocess.run(
        [_npm_cmd(), "run", "build:objc3c-native"],
        cwd=ROOT,
        check=True,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 2

    cases = {case["case_id"]: case for case in payload["dynamic_cases"]}
    positive = cases["M253-D001-CASE-POSITIVE-OBJECT-RETENTION"]
    assert positive["process_exit_code"] == 0
    assert positive["backend"] == "llvm-direct"
    assert contract.BOUNDARY_COMMENT_PREFIX in positive["boundary_line"]
    assert set(contract.EXPECTED_SECTION_NAMES).issubset(set(positive["section_names"]))
    assert positive["forward_extension_section_names"] == sorted(contract.D002_EXTENSION_SECTION_NAMES)
    assert positive["unexpected_section_names"] == []
    assert positive["tracked_symbol_offsets"]["__objc3_sec_class_descriptors"] > 0
    assert positive["tracked_symbol_offsets"]["__objc3_sec_category_descriptors"] == 0

    negative = cases["M253-D001-CASE-NEGATIVE-COMPILE-FAILURE"]
    assert negative["process_exit_code"] != 0
    assert negative["manifest_exists"] is False
    assert negative["object_exists"] is False
    assert negative["backend_exists"] is False
    assert negative["diagnostics_json_exists"] is True
    assert negative["diagnostics_txt_exists"] is True
