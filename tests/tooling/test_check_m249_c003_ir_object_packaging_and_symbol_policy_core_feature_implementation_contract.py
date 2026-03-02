from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_all_occurrences(text: str, old: str, new: str) -> str:
    count = text.count(old)
    assert count > 0, f"expected snippet not found for drift mutation: {old}"
    return text.replace(old, new)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m249-c003-ir-object-packaging-symbol-policy-core-feature-implementation-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 39
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m249_c003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m249/M249-C003/")


def test_contract_fails_closed_when_expectations_drop_mandatory_scope_wording(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m249_ir_object_packaging_and_symbol_policy_core_feature_implementation_c003_expectations.md"
    drift_doc.write_text(
        replace_all_occurrences(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "improvements as mandatory scope inputs.",
            "improvements as optional scope inputs.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-C003-DOC-EXP-05" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_dependency_drifts_from_c002(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_packet.md"
    drift_packet.write_text(
        replace_all_occurrences(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "M249-C002",
            "M249-C099",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-C003-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_c002_contract_dependency_id_drifts(tmp_path: Path) -> None:
    drift_c002_doc = tmp_path / "m249_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_c002_expectations.md"
    drift_c002_doc.write_text(
        replace_all_occurrences(
            contract.DEFAULT_C002_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Contract ID: `objc3c-ir-object-packaging-symbol-policy-modular-split-scaffolding/m249-c002-v1`",
            "Contract ID: `objc3c-ir-object-packaging-symbol-policy-modular-split-scaffolding/m249-c002-drift`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--c002-expectations-doc",
            str(drift_c002_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-C003-C002-01" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_execution_replay_optimization_input(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_all_occurrences(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"test:objc3c:execution-replay-proof": ',
            '"test:objc3c:execution-replay-disabled": ',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-C003-PKG-05" for failure in payload["failures"])

