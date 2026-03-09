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
    / "check_m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m263_b001_bootstrap_legality_duplicate_policy_and_failure_contract_and_architecture_freeze",
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
    assert payload["checks_total"] >= 35
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["next_implementation_issue"] == "M263-B002"
    assert payload["failures"] == []


def test_default_summary_out_is_under_tmp_reports_m263_b001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out.resolve()).replace("\\", "/")
    assert normalized.endswith(
        "/tmp/reports/m263/M263-B001/bootstrap_legality_duplicate_policy_and_failure_contract_summary.json"
    )


def test_checker_fails_closed_when_expectations_drop_duplicate_policy(tmp_path: Path) -> None:
    original = contract.STATIC_SNIPPETS[contract.EXPECTATIONS_DOC]
    try:
        contract.STATIC_SNIPPETS[contract.EXPECTATIONS_DOC] = tuple(
            contract.SnippetCheck(
                item.check_id,
                "`duplicate-registration-policy-v2`"
                if item.check_id == "M263-B001-DOC-EXP-05"
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
        failure["check_id"] == "M263-B001-DOC-EXP-05"
        for failure in payload["failures"]
    )


def test_checker_passes_dynamic(tmp_path: Path) -> None:
    if not contract.NATIVE_EXE.exists():
        pytest.skip("native frontend is not built")
    if _native_is_stale():
        pytest.skip("native frontend is older than the B001 source inputs")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes"]["skipped"] is False

    explicit = payload["dynamic_probes"]["explicit"]
    default = payload["dynamic_probes"]["default"]
    assert explicit["registration_descriptor_identifier"] == "BootstrapLegalityDescriptor"
    assert explicit["image_root_identifier"] == "BootstrapLegalityImageRoot"
    assert explicit["registration_descriptor_identity_source"] == "source-pragma"
    assert explicit["image_root_identity_source"] == "source-pragma"
    assert explicit["translation_unit_registration_order_ordinal"] == 1
    assert default["registration_descriptor_identifier"] == "BootstrapLegalityDefault_registration_descriptor"
    assert default["image_root_identifier"] == "BootstrapLegalityDefault_image_root"
    assert default["registration_descriptor_identity_source"] == "module-derived-default"
    assert default["image_root_identity_source"] == "module-derived-default"
    assert default["translation_unit_registration_order_ordinal"] == 1
