from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m253_b003_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m253_b003_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m253_b003_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources_static_only(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--skip-dynamic-probes", "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m253-b003-coff-elf-and-mach-o-metadata-policy-surface-core-feature-expansion-v1"
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is False
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m253_b003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m253/M253-B003/")


def test_contract_fails_closed_when_expectations_drop_mach_o_token(tmp_path: Path) -> None:
    drift_doc = tmp_path / "expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "`mach-o`",
            "`macho`",
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
    assert any(failure["check_id"] == "M253-B003-DOC-EXP-05" for failure in payload["failures"])


def test_contract_fails_closed_when_lowering_header_drops_surface_contract_id(tmp_path: Path) -> None:
    drift_header = tmp_path / "objc3_lowering_contract.h"
    drift_header.write_text(
        contract.DEFAULT_LOWERING_HEADER.read_text(encoding="utf-8").replace(
            "kObjc3RuntimeMetadataObjectFormatSurfaceContractId",
            "kObjc3RuntimeMetadataObjectFormatContractId",
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
    assert any(failure["check_id"] == "M253-B003-LHDR-01" for failure in payload["failures"])


def test_dynamic_probe_records_host_object_format_policy(tmp_path: Path) -> None:
    if not contract.DEFAULT_NATIVE_EXE.exists():
        pytest.skip("native objc3c binary is not built")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is True
    assert payload["dynamic_probes_executed"] is True
    assert len(payload["dynamic_cases"]) == 1

    case = payload["dynamic_cases"][0]
    host_policy = payload["host_policy"]
    assert case["process_exit_code"] == 0
    assert case["detected_object_format"] == host_policy["object_format"]
    assert f"object_format_contract={contract.CONTRACT_ID}" in case["policy_line"]
    assert f"object_format={host_policy['object_format']}" in case["policy_line"]
    assert f"section_spelling_model={host_policy['section_spelling_model']}" in case["policy_line"]
    assert f"retention_anchor_model={host_policy['retention_anchor_model']}" in case["policy_line"]
    assert "@llvm.used = appending global [" in case["llvm_used_line"]
