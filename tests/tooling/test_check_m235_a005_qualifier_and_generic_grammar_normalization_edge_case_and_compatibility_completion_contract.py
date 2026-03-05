from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m235_a005_qualifier_and_generic_grammar_normalization_edge_case_and_compatibility_completion_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m235-a005-qualifier-and-generic-grammar-normalization-edge-case-and-compatibility-completion-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m235_a005() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m235/M235-A005/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m235_a005_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Dependencies: `M235-A004`",
            "Dependencies: `M235-A099`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-A005-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drops_a004(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8").replace(
            '"check:objc3c:m235-a005-lane-a-readiness": '
            '"npm run check:objc3c:m235-a004-lane-a-readiness '
            '&& npm run check:objc3c:m235-a005-qualifier-and-generic-grammar-normalization-edge-case-and-compatibility-completion-contract '
            '&& npm run test:tooling:m235-a005-qualifier-and-generic-grammar-normalization-edge-case-and-compatibility-completion-contract"',
            '"check:objc3c:m235-a005-lane-a-readiness": '
            '"npm run check:objc3c:m235-a005-qualifier-and-generic-grammar-normalization-edge-case-and-compatibility-completion-contract '
            '&& npm run test:tooling:m235-a005-qualifier-and-generic-grammar-normalization-edge-case-and-compatibility-completion-contract"',
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-A005-PKG-03" for failure in payload["failures"])



