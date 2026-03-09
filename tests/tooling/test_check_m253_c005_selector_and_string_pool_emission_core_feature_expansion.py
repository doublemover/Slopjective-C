from __future__ import annotations

import importlib.util
import json
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m253_c005_selector_and_string_pool_emission_core_feature_expansion.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m253_c005_selector_and_string_pool_emission_core_feature_expansion",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m253_c005_selector_and_string_pool_emission_core_feature_expansion.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m253-c005-selector-and-string-pool-emission-core-feature-expansion-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 35
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m253_c005() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m253/M253-C005/")


def test_contract_fails_closed_when_expectations_drop_contract_id(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            f"Contract ID: `{contract.CONTRACT_ID}`",
            "Contract ID: `objc3c-runtime-selector-string-pool-emission/changed`",
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
    assert any(failure["check_id"] == "M253-C005-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_ir_emitter_drops_selector_string_pool_named_metadata(tmp_path: Path) -> None:
    drift_ir = tmp_path / "objc3_ir_emitter.cpp"
    drift_ir.write_text(
        contract.DEFAULT_IR_EMITTER.read_text(encoding="utf-8").replace(
            'out << "!objc3.objc_runtime_selector_string_pool_emission = !{!59}\\n";',
            'out << "!objc3.objc_runtime_selector_string_pools = !{!59}\\n";',
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
    assert any(failure["check_id"] == "M253-C005-IR-01" for failure in payload["failures"])


def test_dynamic_probe_records_canonical_selector_and_string_pool_payloads(tmp_path: Path) -> None:
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
    assert len(payload["dynamic_cases"]) == 2

    cases = {case["case_id"]: case for case in payload["dynamic_cases"]}
    metadata_case = cases["M253-C005-CASE-METADATA"]
    assert metadata_case["process_exit_code"] == 0
    assert metadata_case["backend"] == "llvm-direct"
    assert metadata_case["diagnostics_is_empty"] is True
    assert metadata_case["selector_pool_count"] == 4
    assert metadata_case["string_pool_count"] == 26
    assert metadata_case["objc3.runtime.selector_pool_raw_size"] is not None and metadata_case["objc3.runtime.selector_pool_raw_size"] >= 64
    assert metadata_case["objc3.runtime.string_pool_raw_size"] is not None and metadata_case["objc3.runtime.string_pool_raw_size"] >= 512
    assert contract.BOUNDARY_COMMENT_PREFIX in metadata_case["boundary_line"]

    message_case = cases["M253-C005-CASE-MESSAGE-SEND"]
    assert message_case["process_exit_code"] == 0
    assert message_case["backend"] == "llvm-direct"
    assert message_case["diagnostics_is_empty"] is True
    assert message_case["selector_pool_count"] == 1
    assert message_case["string_pool_count"] == 0
    assert message_case["objc3.runtime.selector_pool_raw_size"] is not None and message_case["objc3.runtime.selector_pool_raw_size"] >= 24
    assert message_case["objc3.runtime.string_pool_raw_size"] is not None and message_case["objc3.runtime.string_pool_raw_size"] >= 8
    assert contract.BOUNDARY_COMMENT_PREFIX in message_case["boundary_line"]
