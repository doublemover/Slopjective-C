from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py"
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
    assert payload["mode"] == "m245-c004-lowering-ir-portability-contracts-core-feature-expansion-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 43
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m245_c004() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m245/M245-C004/")


def test_contract_emits_json_when_requested(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    payload = json.loads(capsys.readouterr().out)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_expectations_issue_anchor_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m245_lowering_ir_portability_contracts_core_feature_expansion_c004_expectations.md"
    drift_doc.write_text(
        replace_all_occurrences(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Issue `#6639` defines canonical lane-C core-feature expansion scope.",
            "Issue `#6999` defines canonical lane-C core-feature expansion scope.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-C004-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_dependency_token_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m245_c004_lowering_ir_portability_contracts_core_feature_expansion_packet.md"
    drift_packet.write_text(
        replace_all_occurrences(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M245-C003`",
            "Dependencies: `M245-C099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-C004-DOC-PKT-04" for failure in payload["failures"])


def test_contract_fails_closed_when_c003_contract_id_drifts(tmp_path: Path) -> None:
    drift_c003_doc = tmp_path / "m245_lowering_ir_portability_contracts_core_feature_implementation_c003_expectations.md"
    drift_c003_doc.write_text(
        replace_all_occurrences(
            contract.DEFAULT_C003_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Contract ID: `objc3c-lowering-ir-portability-contracts-core-feature-implementation/m245-c003-v1`",
            "Contract ID: `objc3c-lowering-ir-portability-contracts-core-feature-implementation/m245-c003-drift`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--c003-expectations-doc",
            str(drift_c003_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-C004-C003-01" for failure in payload["failures"])


def test_contract_fails_closed_when_package_chain_drops_c003_readiness(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    package_payload = json.loads(contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"))
    scripts = package_payload["scripts"]
    scripts["check:objc3c:m245-c004-lane-c-readiness"] = (
        "npm run check:objc3c:m245-c004-lowering-ir-portability-contracts-core-feature-expansion-contract "
        "&& npm run test:tooling:m245-c004-lowering-ir-portability-contracts-core-feature-expansion-contract"
    )
    drift_package.write_text(json.dumps(package_payload, indent=2) + "\n", encoding="utf-8")

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-C004-PKG-03" for failure in payload["failures"])
