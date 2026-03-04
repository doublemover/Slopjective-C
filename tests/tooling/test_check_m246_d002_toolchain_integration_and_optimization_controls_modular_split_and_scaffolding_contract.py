from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_once(text: str, old: str, new: str) -> str:
    assert old in text
    return text.replace(old, new, 1)


def test_contract_argument_parsing_defaults_under_tmp_reports() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-D002/")
    assert args.d001_checker == contract.DEFAULT_D001_CHECKER
    assert args.d001_test == contract.DEFAULT_D001_TEST


def test_contract_passes_on_repository_sources_and_emits_summary_contract(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert set(payload) == {"mode", "ok", "checks_total", "checks_passed", "failures"}
    assert (
        payload["mode"]
        == "m246-d002-toolchain-integration-optimization-controls-modular-split-and-scaffolding-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 50
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_fails_closed_when_readiness_chain_drops_d001_checker(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m246_d002_lane_d_readiness.py"
    drift_readiness.write_text(
        replace_once(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/check_m246_d001_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_contract.py",
            "scripts/check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-D002-RDY-02" for failure in payload["failures"])


def test_contract_failure_summary_contract_contains_finding_shape(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m246_d002_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Issue `#5107` defines canonical lane-D modular split and scaffolding scope.",
            "Issue `#5999` defines canonical lane-D modular split and scaffolding scope.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert set(payload) == {"mode", "ok", "checks_total", "checks_passed", "failures"}
    assert payload["ok"] is False
    assert payload["checks_passed"] < payload["checks_total"]
    assert payload["failures"]
    assert all(set(failure) == {"artifact", "check_id", "detail"} for failure in payload["failures"])
