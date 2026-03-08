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
    / "check_m254_c002_constructor_and_init_stub_emission_core_feature_implementation.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m254_c002_constructor_and_init_stub_emission_core_feature_implementation",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {SCRIPT_PATH}")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _native_is_stale() -> bool:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        return True
    native_mtime = contract.DEFAULT_NATIVE_EXE.stat().st_mtime
    freshness_inputs = (
        contract.DEFAULT_IR_EMITTER,
        contract.DEFAULT_FRONTEND_ARTIFACTS,
        contract.DEFAULT_DRIVER_CPP,
        contract.DEFAULT_PROCESS_CPP,
        contract.DEFAULT_FIXTURE,
        contract.DEFAULT_RUNTIME_PROBE,
    )
    return any(path.stat().st_mtime > native_mtime for path in freshness_inputs)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m254-c002-constructor-init-stub-emission-core-feature-implementation-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m254_c002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m254/M254-C002/")


def test_contract_fails_closed_when_expectations_drop_global_ctors(tmp_path: Path) -> None:
    drift_doc = tmp_path / contract.DEFAULT_EXPECTATIONS_DOC.name
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`@llvm.global_ctors`",
            "`@llvm.global_bootstrap_roots`",
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
        failure["check_id"] == "M254-C002-DOC-EXP-03"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_ir_emitter_drops_bootstrap_function(tmp_path: Path) -> None:
    drift_ir = tmp_path / "objc3_ir_emitter.cpp"
    drift_ir.write_text(
        contract.DEFAULT_IR_EMITTER.read_text(encoding="utf-8").replace(
            "EmitRuntimeBootstrapLoweringFunctions(",
            "EmitRuntimeBootstrapCtorFunctions(",
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
    assert any(
        failure["check_id"] == "M254-C002-IR-03"
        for failure in payload["failures"]
    )


def test_dynamic_probe_covers_live_constructor_startup_path(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")
    if _native_is_stale():
        pytest.skip("native frontend is older than the C002 source inputs")
    if not contract.RUNTIME_LIBRARY.exists():
        pytest.skip("runtime library is not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 1

    case = payload["dynamic_cases"][0]
    assert case["registration_manifest_path"].endswith(
        "module.runtime-registration-manifest.json"
    )
    assert case["constructor_init_stub_symbol"].startswith(contract.INIT_STUB_PREFIX)
    assert case["bootstrap_registration_table_symbol"].startswith(
        contract.REGISTRATION_TABLE_PREFIX
    )
    assert contract.COFF_STARTUP_SECTION in case["sections"]

    probe_payload = case["probe_payload"]
    assert probe_payload["copy_status"] == 0
    assert probe_payload["registered_image_count"] == 1
    assert probe_payload["last_registration_status"] == 0
    assert (
        probe_payload["last_registered_translation_unit_identity_key"]
        == case["translation_unit_identity_key"]
    )
