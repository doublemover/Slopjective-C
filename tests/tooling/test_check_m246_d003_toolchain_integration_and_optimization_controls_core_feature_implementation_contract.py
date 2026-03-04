from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_once(text: str, old: str, new: str) -> str:
    assert old in text
    return text.replace(old, new, 1)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m246-d003-toolchain-integration-optimization-controls-core-feature-implementation-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 25
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m246_d003() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-D003/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m246_d003_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M246-D002`",
            "Dependencies: `M246-D099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-D003-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m246_d003_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#5108`",
            "Issue: `#5000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-D003-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_drops_pytest_target(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m246_d003_lane_d_readiness.py"
    drift_readiness.write_text(
        replace_once(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "tests/tooling/test_check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py",
            "tests/tooling/test_check_m246_d003_missing_target_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-D003-RDY-03" for failure in payload["failures"])
