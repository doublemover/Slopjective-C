from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m253_d002_linker_retention_anchors_and_dead_strip_resistance_core_feature_implementation.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m253_d002_linker_retention_anchors_and_dead_strip_resistance_core_feature_implementation",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {SCRIPT_PATH}")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _native_binary_is_stale() -> bool:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        return True
    binary_mtime = contract.DEFAULT_NATIVE_EXE.stat().st_mtime
    freshness_inputs = (
        contract.DEFAULT_LOWERING_HEADER,
        contract.DEFAULT_LOWERING_CPP,
        contract.DEFAULT_IR_EMITTER,
        contract.DEFAULT_PROCESS_HEADER,
        contract.DEFAULT_PROCESS_CPP,
        contract.DEFAULT_MANIFEST_ARTIFACTS_HEADER,
        contract.DEFAULT_MANIFEST_ARTIFACTS_CPP,
        contract.DEFAULT_DRIVER_CPP,
        contract.DEFAULT_FRONTEND_ANCHOR,
    )
    return any(path.stat().st_mtime > binary_mtime for path in freshness_inputs)


def _toolchain_ready() -> bool:
    return (
        contract.resolve_tool(contract.DEFAULT_LLVM_READOBJ) is not None
        and contract.resolve_tool(contract.DEFAULT_LLVM_OBJDUMP) is not None
        and contract.resolve_tool(contract.DEFAULT_CLANG) is not None
        and (
            contract.resolve_tool(contract.DEFAULT_LLVM_LIB) is not None
            or contract.resolve_tool(contract.DEFAULT_LLVM_AR) is not None
        )
    )


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m253-d002-linker-retention-and-dead-strip-resistance-core-feature-implementation-v1"
    )
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m253_d002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m253/M253-D002/")


def test_contract_fails_closed_when_expectations_drop_response_artifact(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            f"`module{contract.RESPONSE_SUFFIX}`",
            "`module.runtime-metadata-linker-flags.rsp`",
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
    assert any(failure["check_id"] == "M253-D002-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_process_loses_boundary_prefix(tmp_path: Path) -> None:
    drift_process = tmp_path / "objc3_process.cpp"
    drift_process.write_text(
        contract.DEFAULT_PROCESS_CPP.read_text(encoding="utf-8").replace(
            "; runtime_metadata_linker_retention = ",
            "; runtime_metadata_linker_boundary = ",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--process-cpp",
            str(drift_process),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M253-D002-PCPP-01" for failure in payload["failures"])


def test_dynamic_probe_proves_single_library_retention(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binary is not built")
    if _native_binary_is_stale():
        pytest.skip("native objc3c binary is older than the D002 source inputs")
    if not _toolchain_ready():
        pytest.skip("clang/llvm inspection tools are not available")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 2

    positive_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M253-D002-CASE-POSITIVE-SINGLE-LIBRARY-RETENTION")
    negative_case = next(case for case in payload["dynamic_cases"] if case["case_id"] == "M253-D002-CASE-NEGATIVE-FAIL-CLOSED")

    assert positive_case["compile_exit_code"] == 0
    assert positive_case["backend"] == "llvm-direct"
    assert positive_case["response_file"].endswith(contract.RESPONSE_SUFFIX)
    assert positive_case["discovery_file"].endswith(contract.DISCOVERY_SUFFIX)
    assert positive_case["linker_anchor_symbol"].startswith("objc3_runtime_metadata_link_anchor_")
    assert positive_case["discovery_root_symbol"].startswith("objc3_runtime_metadata_discovery_root_")
    assert positive_case["plain_link_exit_code"] == 0
    assert positive_case["retained_link_exit_code"] == 0
    assert positive_case["plain_metadata_sections"] == []
    assert positive_case["retained_metadata_sections"]
    assert positive_case["linked_metadata_section_prefix"] in {"objc3.ru", "objc3.runtime"}

    assert negative_case["compile_exit_code"] != 0
    assert negative_case["object_exists"] is False
    assert negative_case["backend_exists"] is False
    assert negative_case["response_exists"] is False
    assert negative_case["discovery_exists"] is False
