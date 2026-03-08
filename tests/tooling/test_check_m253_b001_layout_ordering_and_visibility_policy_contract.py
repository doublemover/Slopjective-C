from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m253_b001_layout_ordering_and_visibility_policy_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m253_b001_layout_ordering_and_visibility_policy_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m253_b001_layout_ordering_and_visibility_policy_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m253-b001-layout-ordering-and-visibility-policy-contract-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m253_b001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m253/M253-B001/")


def test_contract_fails_closed_when_expectations_drop_visibility_policy(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`local-linkage-omits-explicit-ir-visibility`",
            "`explicit-hidden-visibility`",
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
    assert any(failure["check_id"] == "M253-B001-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_lowering_header_drops_contract_id_constant(tmp_path: Path) -> None:
    drift_header = tmp_path / "objc3_lowering_contract.h"
    drift_header.write_text(
        contract.DEFAULT_LOWERING_HEADER.read_text(encoding="utf-8").replace(
            "kObjc3RuntimeMetadataLayoutOrderingVisibilityPolicyContractId",
            "kObjc3RuntimeMetadataOrderingVisibilityPolicyContractId",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--lowering-header",
            str(drift_header),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M253-B001-LHDR-01" for failure in payload["failures"])


def test_dynamic_probes_cover_manifest_matrix_and_emitted_ir_policy(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists() or not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binaries are not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 2

    runner_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M253-B001-CASE-RUNNER")
    native_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M253-B001-CASE-NATIVE")

    assert runner_case["process_exit_code"] == 0
    assert runner_case["matrix_present"] is True
    assert runner_case["row_ordering_model"] == "source-graph-node-kind-order-v1"
    assert runner_case["row_keys"] == contract.MATRIX_ROW_KEYS

    assert native_case["process_exit_code"] == 0
    assert native_case["contains_comdat"] is False
    assert "@llvm.used = appending global [" in native_case["llvm_used_line"]
    positions = [native_case["symbol_positions"][symbol] for symbol in contract.FAMILY_ORDER]
    assert positions == sorted(positions)
