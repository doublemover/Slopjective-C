from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m226_b001_parser_sema_handoff_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m226_b001_parser_sema_handoff_contract",
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
    assert payload["mode"] == "m226-b001-parser-sema-handoff-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_fails_closed_when_pipeline_drops_program_handoff(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    pipeline_drift = tmp_path / "objc3_frontend_pipeline.cpp"
    pipeline_drift.write_text(
        (ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp")
        .read_text(encoding="utf-8")
        .replace(
            "sema_input.program = &result.program;",
            "sema_input.program = nullptr;",
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["pipeline_source"] = pipeline_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)
    monkeypatch.setattr(
        contract,
        "ARTIFACT_ORDER",
        tuple(artifact_overrides.keys()),
    )
    monkeypatch.setattr(
        contract,
        "ARTIFACT_RANK",
        {name: index for index, name in enumerate(contract.ARTIFACT_ORDER)},
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-B001-PIP-06" for failure in payload["failures"]
    )


def test_contract_fails_closed_when_pipeline_includes_parser_internal_header(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    pipeline_drift = tmp_path / "objc3_frontend_pipeline.cpp"
    pipeline_drift.write_text(
        (ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_pipeline.cpp")
        .read_text(encoding="utf-8")
        .replace(
            '#include "parse/objc3_ast_builder_contract.h"',
            '#include "parse/objc3_parser.h"\n#include "parse/objc3_ast_builder_contract.h"',
            1,
        ),
        encoding="utf-8",
    )

    artifact_overrides = dict(contract.ARTIFACTS)
    artifact_overrides["pipeline_source"] = pipeline_drift
    monkeypatch.setattr(contract, "ARTIFACTS", artifact_overrides)
    monkeypatch.setattr(
        contract,
        "ARTIFACT_ORDER",
        tuple(artifact_overrides.keys()),
    )
    monkeypatch.setattr(
        contract,
        "ARTIFACT_RANK",
        {name: index for index, name in enumerate(contract.ARTIFACT_ORDER)},
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M226-B001-FORB-01"
        for failure in payload["failures"]
    )

