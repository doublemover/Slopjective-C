from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_a006_cstyle_pointer_diagnostic_precision_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_a006_cstyle_pointer_diagnostic_precision_contract",
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
    assert payload["mode"] == "m226-cstyle-pointer-diagnostic-precision-contract-a006-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_pointer_label_is_removed(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_source = tmp_path / "objc3_parser.cpp"
    drift_source.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "parse" / "objc3_parser.cpp"
        ).read_text(encoding="utf-8").replace(
            "const char *pointer_type = type == ValueType::I32 ? \"i32*\" : \"bool*\";",
            "const char *pointer_type = \"<unknown*>\";",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["parser_source"] = drift_source
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-A006-PARSE-01"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_doc_id_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m226_c_style_pointer_diagnostic_precision_expectations.md"
    drift_doc.write_text(
        (
            ROOT / "docs" / "contracts" / "m226_c_style_pointer_diagnostic_precision_expectations.md"
        ).read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-cstyle-pointer-diagnostic-precision-contract/m226-a006-v1`",
            "Contract ID: `objc3c-cstyle-pointer-diagnostic-precision-contract/m226-a006-drift`",
            1,
        ),
        encoding="utf-8",
    )
    artifacts = dict(contract.ARTIFACTS)
    artifacts["contract_doc"] = drift_doc
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])
    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-A006-DOC-01"
        for failure in payload["failures"]
    )
