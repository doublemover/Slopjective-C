from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m253_c003_protocol_and_category_data_emission_core_feature_implementation.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m253_c003_protocol_and_category_data_emission_core_feature_implementation",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m253_c003_protocol_and_category_data_emission_core_feature_implementation.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m253-c003-protocol-and-category-data-emission-core-feature-implementation-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m253_c003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m253/M253-C003/")


def test_contract_fails_closed_when_expectations_drop_protocol_payload_model(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`protocol-descriptor-bundles-with-inherited-protocol-ref-lists`",
            "`protocol-bundles`",
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
    assert any(failure["check_id"] == "M253-C003-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_ir_emitter_drops_protocol_category_named_metadata(tmp_path: Path) -> None:
    drift_ir = tmp_path / "objc3_ir_emitter.cpp"
    drift_ir.write_text(
        contract.DEFAULT_IR_EMITTER.read_text(encoding="utf-8").replace(
            'out << "!objc3.objc_runtime_protocol_category_emission = !{!57}\\n";',
            'out << "!objc3.objc_runtime_protocol_category = !{!57}\\n";',
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
    assert any(failure["check_id"] == "M253-C003-IR-01" for failure in payload["failures"])


def test_dynamic_probe_records_real_protocol_and_category_section_payloads(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binary is not built")

    subprocess.run(
        ["pwsh", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", "scripts/build_objc3c_native.ps1"],
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
    assert len(payload["dynamic_cases"]) == 1

    case = payload["dynamic_cases"][0]
    assert case["process_exit_code"] == 0
    assert case["backend"] == "llvm-direct"
    assert case["diagnostics_is_empty"] is True
    assert case["protocol_bundle_count"] == 2
    assert case["category_bundle_count"] == 2
    assert case["inherited_protocol_ref_list_count"] == 2
    assert case["adopted_protocol_ref_list_count"] == 2
    assert case["category_attachment_list_count"] == 2
    assert contract.BOUNDARY_COMMENT_PREFIX in case["boundary_line"]
    assert case["protocol_section_raw_size"] is not None and case["protocol_section_raw_size"] > 64
    assert case["protocol_section_relocations"] is not None and case["protocol_section_relocations"] >= 4
    assert case["category_section_raw_size"] is not None and case["category_section_raw_size"] > 128
    assert case["category_section_relocations"] is not None and case["category_section_relocations"] >= 8
