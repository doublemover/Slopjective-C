from __future__ import annotations

import importlib.util
import json
import re
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m227_b006_type_system_objc3_forms_edge_robustness_contract",
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
    assert payload["mode"] == "m227-type-system-objc3-forms-edge-robustness-b006-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 15
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_unknown_exclusion_guard_is_removed(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_source = tmp_path / "objc3_type_form_scaffold.cpp"
    source_text = (
        ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.cpp"
    ).read_text(encoding="utf-8")
    mutated_source, substitution_count = re.subn(
        r"form == ValueType::Unknown",
        "false /* drifted unknown exclusion guard */",
        source_text,
        count=1,
        flags=re.MULTILINE,
    )
    assert substitution_count == 1
    drift_source.write_text(
        mutated_source,
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["scaffold_source"] = drift_source
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-B006-SRC-11" for failure in payload["failures"])


def test_contract_fails_closed_when_reference_sel_flag_is_removed(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_header = tmp_path / "objc3_type_form_scaffold.h"
    drift_header.write_text(
        (
            ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_type_form_scaffold.h"
        ).read_text(encoding="utf-8").replace(
            "  bool canonical_reference_includes_sel = false;\n",
            "",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["scaffold_header"] = drift_header
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M227-B006-HDR-01" for failure in payload["failures"])
