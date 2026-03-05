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
    / "check_m234_c015_accessor_and_ivar_lowering_contracts_advanced_core_workpack_shard_1_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m234_c015_accessor_and_ivar_lowering_contracts_advanced_core_workpack_shard_1_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m234_c015_accessor_and_ivar_lowering_contracts_advanced_core_workpack_shard_1_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m234-c015-accessor-and-ivar-lowering-contracts-advanced-core-workpack-shard-1-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 38
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m234_c015() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m234/M234-C015/")


def test_contract_emits_json_when_requested(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    payload = json.loads(capsys.readouterr().out)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m234_c015_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M234-C014`",
            "Dependencies: `M234-C099`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M234-C015-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drops_c014(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m234-c015-lane-c-readiness": '
            '"npm run check:objc3c:m234-c014-lane-c-readiness '
            '&& npm run check:objc3c:m234-c015-accessor-and-ivar-lowering-contracts-advanced-core-workpack-shard-1-contract '
            '&& npm run test:tooling:m234-c015-accessor-and-ivar-lowering-contracts-advanced-core-workpack-shard-1-contract"',
            '"check:objc3c:m234-c015-lane-c-readiness": '
            '"npm run check:objc3c:m234-c015-accessor-and-ivar-lowering-contracts-advanced-core-workpack-shard-1-contract '
            '&& npm run test:tooling:m234-c015-accessor-and-ivar-lowering-contracts-advanced-core-workpack-shard-1-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M234-C015-PKG-03" for failure in payload["failures"])






