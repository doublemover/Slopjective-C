from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_a021_parser_advanced_core_workpack_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_a021_parser_advanced_core_workpack_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m226_a021_parser_advanced_core_workpack_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m226-parser-advanced-core-workpack-contract-a021-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_parser_contract_snippet_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_contract = tmp_path / "objc3_parser_contract.h"
    drift_contract.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser_contract.h"
        ).read_text(encoding="utf-8").replace(
            "std::size_t protocol_method_decl_count = 0;",
            "std::size_t protocol_method_decl_count_drift = 0;",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["parser_contract"] = drift_contract
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M226-A021-PRS-02" for failure in payload["failures"])
