from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT / "scripts" / "check_m246_a003_frontend_optimization_hint_capture_core_feature_implementation_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_a003_frontend_optimization_hint_capture_core_feature_implementation_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_a003_frontend_optimization_hint_capture_core_feature_implementation_contract.py"
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
    assert payload["mode"] == "m246-a003-frontend-optimization-hint-capture-core-feature-implementation-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m246_a003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-A003/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m246_a003_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-A002`",
            "Dependencies: `M246-A099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-A003-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_run_chain_drops_a002_dependency(tmp_path: Path) -> None:
    drift_run_script = tmp_path / "run_m246_a003_lane_a_readiness.py"
    drift_run_script.write_text(
        replace_all(
            contract.DEFAULT_RUN_SCRIPT.read_text(encoding="utf-8"),
            "check:objc3c:m246-a002-lane-a-readiness",
            "check:objc3c:m246-a099-lane-a-readiness",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--run-script", str(drift_run_script), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-A003-RUN-01" for failure in payload["failures"])


def test_contract_fails_closed_when_a002_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_a002_checker.py"
    exit_code = contract.run(
        [
            "--a002-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-A003-DEP-A002-ARG-01" for failure in payload["failures"])
