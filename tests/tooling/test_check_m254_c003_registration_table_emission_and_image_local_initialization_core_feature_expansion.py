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
    / "check_m254_c003_registration_table_emission_and_image_local_initialization_core_feature_expansion.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m254_c003_registration_table_emission_and_image_local_initialization_core_feature_expansion",
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
        contract.DEFAULT_IR_EMITTER_HEADER,
        contract.DEFAULT_FRONTEND_TYPES,
        contract.DEFAULT_FRONTEND_ARTIFACTS,
        contract.DEFAULT_DRIVER_CPP,
        contract.DEFAULT_PROCESS_HEADER,
        contract.DEFAULT_PROCESS_CPP,
        contract.DEFAULT_FRONTEND_ANCHOR_CPP,
        contract.DEFAULT_PRIMARY_FIXTURE,
        contract.DEFAULT_CATEGORY_FIXTURE,
        contract.DEFAULT_RUNTIME_PROBE,
    )
    return any(path.stat().st_mtime > native_mtime for path in freshness_inputs)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == (
        "m254-c003-registration-table-image-local-initialization-core-feature-expansion-v1"
    )
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m254_c003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m254/M254-C003/")


def test_contract_fails_closed_when_expectations_drop_image_local_init_state_cell(
    tmp_path: Path,
) -> None:
    drift_doc = tmp_path / contract.DEFAULT_EXPECTATIONS_DOC.name
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            contract.IMAGE_LOCAL_INIT_STATE_SYMBOL_PREFIX,
            "__objc3_runtime_bootstrap_latch_",
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
        failure["check_id"] == "M254-C003-DOC-EXP-04"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_ir_emitter_drops_image_local_symbol_helper(
    tmp_path: Path,
) -> None:
    drift_ir = tmp_path / "objc3_ir_emitter.cpp"
    drift_ir.write_text(
        contract.DEFAULT_IR_EMITTER.read_text(encoding="utf-8").replace(
            "RuntimeBootstrapImageLocalInitStateSymbol()",
            "RuntimeBootstrapImageLocalStateSymbol()",
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
        failure["check_id"] == "M254-C003-IR-02"
        for failure in payload["failures"]
    )


def test_dynamic_probe_covers_metadata_and_category_bootstrap_cases(
    tmp_path: Path,
) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")
    if _native_is_stale():
        pytest.skip("native frontend is older than the C003 source inputs")
    if not contract.RUNTIME_LIBRARY.exists():
        pytest.skip("runtime library is not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 2

    cases = {case["case_id"]: case for case in payload["dynamic_cases"]}
    assert set(cases) == {"metadata-library", "category-library"}

    metadata_case = cases["metadata-library"]
    category_case = cases["category-library"]
    assert metadata_case["category_descriptor_count"] == 0
    assert category_case["category_descriptor_count"] > 0

    for case in (metadata_case, category_case):
        assert case["registration_manifest_path"].endswith(
            "module.runtime-registration-manifest.json"
        )
        assert case["registration_table_symbol"].startswith(
            contract.REGISTRATION_TABLE_SYMBOL_PREFIX
        )
        assert case["image_local_init_state_symbol"].startswith(
            contract.IMAGE_LOCAL_INIT_STATE_SYMBOL_PREFIX
        )
        assert contract.COFF_STARTUP_SECTION in case["sections"]
        probe_payload = case["probe_payload"]
        assert probe_payload["copy_status"] == 0
        assert probe_payload["registered_image_count"] == 1
        assert probe_payload["registered_descriptor_total"] == case["descriptor_total"]
        assert probe_payload["last_registration_status"] == 0
        assert (
            probe_payload["last_registered_translation_unit_identity_key"]
            == case["translation_unit_identity_key"]
        )
