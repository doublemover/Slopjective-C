from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_all(text: str, old: str, new: str) -> str:
    assert old in text
    replaced = text.replace(old, new)
    assert old not in replaced
    return replaced


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m243-b003-semantic-diagnostic-taxonomy-and-fixit-synthesis-core-feature-implementation-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 49
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m243_b003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m243/M243-B003/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m243_b003_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M243-B002`",
            "Dependencies: `M243-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-B003-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drops_b002(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_all(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"check:objc3c:m243-b003-lane-b-readiness": '
            '"npm run check:objc3c:m243-b002-lane-b-readiness '
            '&& npm run check:objc3c:m243-b003-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-implementation-contract '
            '&& npm run test:tooling:m243-b003-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-implementation-contract"',
            '"check:objc3c:m243-b003-lane-b-readiness": '
            '"npm run check:objc3c:m243-b003-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-implementation-contract '
            '&& npm run test:tooling:m243-b003-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-implementation-contract"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-B003-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_surface_forces_core_feature_ready(tmp_path: Path) -> None:
    drift_surface = tmp_path / "objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_surface.h"
    drift_surface.write_text(
        contract.DEFAULT_CORE_SURFACE_HEADER.read_text(encoding="utf-8")
        + "\nsurface.core_feature_impl_ready = true;\n",
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        ["--core-surface-header", str(drift_surface), "--summary-out", str(summary_out)]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M243-B003-FORB-01" for failure in payload["failures"])
