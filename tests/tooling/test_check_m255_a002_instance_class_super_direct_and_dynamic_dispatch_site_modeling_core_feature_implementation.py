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
    / "check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


SOURCE_INPUTS = (
    contract.DEFAULT_AST_HEADER,
    contract.DEFAULT_PARSER,
    contract.DEFAULT_FRONTEND_PIPELINE,
    contract.DEFAULT_SEMA,
    contract.DEFAULT_LOWERING_HEADER,
    contract.DEFAULT_LOWERING_CPP,
    contract.DEFAULT_FRONTEND_ARTIFACTS,
    contract.DEFAULT_IR_HEADER,
    contract.DEFAULT_IR_CPP,
)


def _binary_is_stale(binary: Path) -> bool:
    if not binary.exists():
        return True
    binary_mtime = binary.stat().st_mtime
    return any(path.stat().st_mtime > binary_mtime for path in SOURCE_INPUTS)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m255-a002-dispatch-site-modeling-core-feature-implementation-v1"
    assert payload["implementation_contract_id"] == "objc3c-dispatch-site-modeling/m255-a002-v1"
    assert payload["lowering_contract_id"] == "objc3c-dispatch-surface-classification/m255-a001-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 60
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["next_implementation_issue"] == "M255-B001"
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m255_a002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m255/M255-A002/")


def test_contract_fails_closed_when_frontend_pipeline_drops_program_normalization(tmp_path: Path) -> None:
    drift_pipeline = tmp_path / "objc3_frontend_pipeline.cpp"
    drift_pipeline.write_text(
        contract.DEFAULT_FRONTEND_PIPELINE.read_text(encoding="utf-8").replace(
            "NormalizeProgramDispatchSurfaceClassification(\n        MutableObjc3ParsedProgramAst(result.program));",
            "// NormalizeProgramDispatchSurfaceClassification disabled for drift test",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--frontend-pipeline",
            str(drift_pipeline),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M255-A002-PIPE-05" for failure in payload["failures"])


def test_contract_fails_closed_when_ir_emitter_drops_self_binding(tmp_path: Path) -> None:
    drift_ir = tmp_path / "objc3_ir_emitter.cpp"
    drift_ir.write_text(
        contract.DEFAULT_IR_CPP.read_text(encoding="utf-8").replace(
            'ctx.immediate_identifiers["self"] = self_identity;',
            "// self binding removed for drift test",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--ir-cpp",
            str(drift_ir),
            "--skip-dynamic-probes",
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M255-A002-IRC-03" for failure in payload["failures"])


def test_dynamic_probes_cover_runner_and_native_paths(tmp_path: Path) -> None:
    if not contract.DEFAULT_RUNNER_EXE.exists() or not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binaries are not built")
    if _binary_is_stale(contract.DEFAULT_RUNNER_EXE) or _binary_is_stale(contract.DEFAULT_NATIVE_EXE):
        pytest.skip("native binaries are older than the A002 source inputs")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 2

    runner_case = next(case for case in payload["dynamic_cases"] if case["mode"] == "runner-manifest-only")
    native_case = next(case for case in payload["dynamic_cases"] if case["mode"] == "native-llvm-direct")

    assert runner_case["surface"]["instance_dispatch_sites"] == 2
    assert runner_case["surface"]["class_dispatch_sites"] == 2
    assert runner_case["surface"]["super_dispatch_sites"] == 1
    assert runner_case["surface"]["direct_dispatch_sites"] == 0
    assert runner_case["surface"]["dynamic_dispatch_sites"] == 1
    assert runner_case["lowering"]["lane_contract"] == "objc3c-dispatch-surface-classification/m255-a001-v1"

    assert native_case["backend"] == "llvm-direct"
    assert native_case["surface"]["class_dispatch_sites"] == 2
    assert native_case["surface"]["direct_dispatch_sites"] == 0
