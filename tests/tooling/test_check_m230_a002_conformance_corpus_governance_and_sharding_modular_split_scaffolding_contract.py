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
    / "check_m230_a002_conformance_corpus_governance_and_sharding_modular_split_scaffolding_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m230_a002_conformance_corpus_governance_and_sharding_modular_split_scaffolding_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m230_a002_conformance_corpus_governance_and_sharding_modular_split_scaffolding_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m230-a002-conformance-corpus-governance-and-sharding-modular-split-scaffolding-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m230_a002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m230/M230-A002/")


def test_contract_fails_closed_when_expectations_dependency_drifts(tmp_path: Path) -> None:
    drift_doc = (
        tmp_path
        / "m230_conformance_corpus_governance_and_sharding_modular_split_scaffolding_a002_expectations.md"
    )
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M230-A001`",
            "Dependencies: `M230-A999`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M230-A002-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_chain_drops_a001(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m230-a002-lane-a-readiness": '
            '"npm run check:objc3c:m230-a001-lane-a-readiness '
            '&& npm run check:objc3c:m230-a002-conformance-corpus-governance-and-sharding-modular-split-scaffolding-contract '
            '&& npm run test:tooling:m230-a002-conformance-corpus-governance-and-sharding-modular-split-scaffolding-contract"',
            '"check:objc3c:m230-a002-lane-a-readiness": '
            '"npm run check:objc3c:m230-a002-conformance-corpus-governance-and-sharding-modular-split-scaffolding-contract '
            '&& npm run test:tooling:m230-a002-conformance-corpus-governance-and-sharding-modular-split-scaffolding-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M230-A002-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_prerequisite_dependency_is_missing(
    tmp_path: Path,
    monkeypatch: pytest.MonkeyPatch,
) -> None:
    missing_asset = contract.AssetCheck(
        check_id="M230-A002-DEP-MISSING",
        relative_path=Path("scripts/check_m230_a001_conformance_corpus_governance_and_sharding_contract_missing.py"),
    )
    monkeypatch.setattr(contract, "PREREQUISITE_ASSETS", (missing_asset,))
    summary_out = tmp_path / "summary.json"

    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M230-A002-DEP-MISSING" for failure in payload["failures"])

