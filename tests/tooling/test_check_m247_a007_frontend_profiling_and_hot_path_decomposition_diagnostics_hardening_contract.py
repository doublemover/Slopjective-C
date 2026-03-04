from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m247_a007_frontend_profiling_and_hot_path_decomposition_diagnostics_hardening_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_a007_frontend_profiling_and_hot_path_decomposition_diagnostics_hardening_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_a007_frontend_profiling_and_hot_path_decomposition_diagnostics_hardening_contract.py"
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
    assert payload["mode"] == "m247-a007-frontend-profiling-hot-path-decomposition-diagnostics-hardening-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 30
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_a007() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-A007/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_a007_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M247-A006`",
            "Dependencies: `M247-A099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A007-DOC-EXP-05" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_drops_mandatory_scope_wording(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_a007_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "optimization improvements as mandatory scope inputs.",
            "optimization improvements as optional scope inputs.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A007-DOC-PKT-08" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drops_a006_gate(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_all(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"check:objc3c:m247-a007-lane-a-readiness": '
            '"npm run check:objc3c:m247-a006-lane-a-readiness '
            "&& npm run check:objc3c:m247-a007-frontend-profiling-hot-path-decomposition-diagnostics-hardening-contract "
            '&& npm run test:tooling:m247-a007-frontend-profiling-hot-path-decomposition-diagnostics-hardening-contract"',
            '"check:objc3c:m247-a007-lane-a-readiness": '
            '"npm run check:objc3c:m247-a007-frontend-profiling-hot-path-decomposition-diagnostics-hardening-contract '
            '&& npm run test:tooling:m247-a007-frontend-profiling-hot-path-decomposition-diagnostics-hardening-contract"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A007-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_drops_perf_budget_optimization_input(tmp_path: Path) -> None:
    drift_package = tmp_path / "package.json"
    drift_package.write_text(
        replace_all(
            contract.DEFAULT_PACKAGE_JSON.read_text(encoding="utf-8"),
            '"test:objc3c:perf-budget": ',
            '"test:objc3c:perf-budget-disabled": ',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--package-json", str(drift_package), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-A007-PKG-05" for failure in payload["failures"])
