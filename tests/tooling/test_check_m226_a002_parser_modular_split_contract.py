from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_a002_parser_modular_split_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_a002_parser_modular_split_contract",
    SCRIPT_PATH,
)
assert SPEC is not None and SPEC.loader is not None
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m226-parser-modular-split-contract-a002-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 8
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_cmake_drops_support_module(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    cmake_drift = tmp_path / "CMakeLists.txt"
    cmake_drift.write_text(
        (
            ROOT / "native" / "objc3c" / "CMakeLists.txt"
        ).read_text(encoding="utf-8").replace(
            "  src/parse/objc3_parse_support.cpp\n",
            "",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["cmake"] = cmake_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-A002-CMAKE-01"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_parser_reintroduces_local_diag_helper(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    parser_drift = tmp_path / "objc3_parser.cpp"
    parser_drift.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
        ).read_text(encoding="utf-8")
        + "\nstatic std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) { return code; }\n",
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["parser_source"] = parser_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-A002-PARSE-05"
        for failure in payload["failures"]
    )
