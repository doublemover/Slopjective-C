from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m253_c006_binary_inspection_harness_for_emitted_metadata_conformance_corpus_expansion.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m253_c006_binary_inspection_harness_for_emitted_metadata_conformance_corpus_expansion",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m253_c006_binary_inspection_harness_for_emitted_metadata_conformance_corpus_expansion.py"
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
    assert payload["mode"] == "m253-c006-binary-inspection-harness-for-emitted-metadata-conformance-corpus-expansion-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m253_c006() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m253/M253-C006/")


def test_contract_fails_closed_when_expectations_drop_contract_id(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            f"Contract ID: `{contract.CONTRACT_ID}`",
            "Contract ID: `objc3c-runtime-binary-inspection-harness/changed`",
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
    assert any(failure["check_id"] == "M253-C006-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_ir_emitter_drops_binary_inspection_named_metadata(tmp_path: Path) -> None:
    drift_ir = tmp_path / "objc3_ir_emitter.cpp"
    drift_ir.write_text(
        contract.DEFAULT_IR_EMITTER.read_text(encoding="utf-8").replace(
            'out << "!objc3.objc_runtime_binary_inspection_harness = !{!60}\\n";',
            'out << "!objc3.objc_runtime_binary_inspection_corpus = !{!60}\\n";',
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
    assert any(failure["check_id"] == "M253-C006-IR-01" for failure in payload["failures"])


def test_dynamic_probe_records_binary_inspection_corpus(tmp_path: Path) -> None:
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
    assert len(payload["dynamic_cases"]) == 5

    cases = {case["case_id"]: case for case in payload["dynamic_cases"]}
    zero_case = cases["M253-C006-CASE-ZERO-DESCRIPTOR"]
    assert zero_case["process_exit_code"] == 0
    assert zero_case["backend"] == "llvm-direct"
    assert [section["name"] for section in zero_case["sections"]] == [
        "objc3.runtime.category_descriptors",
        "objc3.runtime.class_descriptors",
        "objc3.runtime.image_info",
        "objc3.runtime.ivar_descriptors",
        "objc3.runtime.property_descriptors",
        "objc3.runtime.protocol_descriptors",
    ]

    class_case = cases["M253-C006-CASE-CLASS-PROTOCOL-PROPERTY-IVAR"]
    assert class_case["process_exit_code"] == 0
    assert class_case["backend"] == "llvm-direct"
    assert class_case["tracked_symbol_offsets"]["__objc3_sec_class_descriptors"] > 0
    assert class_case["tracked_symbol_offsets"]["__objc3_sec_category_descriptors"] == 0
    assert contract.BOUNDARY_COMMENT_PREFIX in class_case["boundary_line"]

    category_case = cases["M253-C006-CASE-CATEGORY-PROTOCOL-PROPERTY"]
    assert category_case["process_exit_code"] == 0
    assert category_case["backend"] == "llvm-direct"
    assert category_case["tracked_symbol_offsets"]["__objc3_sec_class_descriptors"] == 0
    assert category_case["tracked_symbol_offsets"]["__objc3_sec_category_descriptors"] > 0

    message_case = cases["M253-C006-CASE-MESSAGE-SEND"]
    assert message_case["process_exit_code"] == 0
    assert message_case["backend"] == "llvm-direct"
    assert message_case["tracked_symbol_offsets"]["__objc3_sec_selector_pool"] > 0
    assert message_case["tracked_symbol_offsets"]["__objc3_sec_string_pool"] == 0

    negative_case = cases["M253-C006-CASE-NEGATIVE-MISSING-INTERFACE-PROPERTY"]
    assert negative_case["process_exit_code"] != 0
    assert negative_case["manifest_exists"] is False
    assert negative_case["object_exists"] is False
    assert negative_case["diagnostics_json_exists"] is True
    assert negative_case["diagnostics_txt_exists"] is True
    assert negative_case["inspection_blocked"] is True
