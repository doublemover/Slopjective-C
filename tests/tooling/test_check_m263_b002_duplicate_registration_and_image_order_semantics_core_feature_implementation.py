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
    / "check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"Unable to load {SCRIPT_PATH}")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def _native_is_stale() -> bool:
    if not contract.NATIVE_EXE.exists():
        return True
    native_mtime = contract.NATIVE_EXE.stat().st_mtime
    freshness_inputs = (
        contract.SEMA_CONTRACT,
        contract.SEMANTIC_PASSES,
        contract.FRONTEND_TYPES,
        contract.FRONTEND_ARTIFACTS_H,
        contract.FRONTEND_ARTIFACTS_CPP,
    )
    return any(path.stat().st_mtime > native_mtime for path in freshness_inputs)


def test_checker_passes_static(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 28
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["next_implementation_issue"] == "M263-B003"
    assert payload["failures"] == []


def test_default_summary_out_is_under_tmp_reports_m263_b002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out.resolve()).replace("\\", "/")
    assert normalized.endswith(
        "/tmp/reports/m263/M263-B002/duplicate_registration_and_image_order_semantics_summary.json"
    )


def test_checker_fails_closed_when_expectations_drop_surface_path(tmp_path: Path) -> None:
    original = contract.STATIC_SNIPPETS[contract.EXPECTATIONS_DOC]
    try:
        contract.STATIC_SNIPPETS[contract.EXPECTATIONS_DOC] = tuple(
            contract.SnippetCheck(
                item.check_id,
                "`frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_missing`"
                if item.check_id == "M263-B002-DOC-EXP-04"
                else item.snippet,
            )
            for item in original
        )
        summary_out = tmp_path / "summary.json"
        exit_code = contract.run(
            ["--summary-out", str(summary_out), "--skip-dynamic-probes"]
        )
    finally:
        contract.STATIC_SNIPPETS[contract.EXPECTATIONS_DOC] = original

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M263-B002-DOC-EXP-04"
        for failure in payload["failures"]
    )


def test_checker_passes_dynamic(tmp_path: Path) -> None:
    if not contract.NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")
    if _native_is_stale():
        pytest.skip("native frontend is older than the B002 source inputs")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes"]["skipped"] is False

    primary = payload["dynamic_probes"]["explicit_primary"]
    rebuild = payload["dynamic_probes"]["explicit_rebuild"]
    peer = payload["dynamic_probes"]["explicit_peer"]
    default = payload["dynamic_probes"]["default"]

    assert primary["translation_unit_identity_key"] == rebuild["translation_unit_identity_key"]
    assert primary["translation_unit_identity_key"] != peer["translation_unit_identity_key"]
    assert primary["registration_descriptor_identifier"] == peer["registration_descriptor_identifier"]
    assert primary["image_root_identifier"] == peer["image_root_identifier"]
    assert default["registration_descriptor_identity_source"] == "module-derived-default"
    assert default["image_root_identity_source"] == "module-derived-default"
