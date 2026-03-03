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
    / "check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m243_a010_diagnostic_grammar_hooks_and_source_precision_conformance_corpus_expansion_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m243-a010-diagnostic-grammar-hooks-and-source-precision-conformance-corpus-expansion-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 35
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_default_summary_out_is_under_tmp_reports_m243_a010() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m243/M243-A010/")


def test_contract_emits_json_when_requested(
    tmp_path: Path,
    capsys: pytest.CaptureFixture[str],
) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out), "--emit-json"])

    assert exit_code == 0
    stdout = capsys.readouterr().out
    payload = json.loads(stdout)
    assert payload["mode"] == contract.MODE
    assert payload["ok"] is True
    assert payload["checks_total"] == payload["checks_passed"]


def test_contract_fails_closed_when_expectations_dependency_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_doc = tmp_path / "m243_a010_expectations.md"
    drift_doc.write_text(
        contract.ARTIFACTS["contract_doc"].read_text(encoding="utf-8").replace(
            "Dependencies: `M243-A009`",
            "Dependencies: `M243-A099`",
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
    assert any(failure["check_id"] == "M243-A010-DOC-05" for failure in payload["failures"])


def test_contract_fails_closed_when_fixture_manifest_case_drifts(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_manifest = tmp_path / "manifest.json"
    drift_manifest.write_text(
        contract.ARTIFACTS["fixture_manifest"].read_text(encoding="utf-8").replace(
            "\"diagnostic_code\": \"O3P114\"",
            "\"diagnostic_code\": \"O3P999\"",
            1,
        ),
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["fixture_manifest"] = drift_manifest
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(
        failure["check_id"] == "M243-A010-FIX-A010-C005"
        for failure in payload["failures"]
    )


def test_contract_fails_closed_when_surface_forces_conformance_corpus_ready(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    drift_surface = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    drift_surface.write_text(
        contract.ARTIFACTS["readiness_surface"].read_text(encoding="utf-8")
        + "\nsurface.parser_diagnostic_grammar_hooks_conformance_corpus_ready = true;\n",
        encoding="utf-8",
    )

    artifacts = dict(contract.ARTIFACTS)
    artifacts["readiness_surface"] = drift_surface
    monkeypatch.setattr(contract, "ARTIFACTS", artifacts)

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-A010-FORB-01" for failure in payload["failures"])
