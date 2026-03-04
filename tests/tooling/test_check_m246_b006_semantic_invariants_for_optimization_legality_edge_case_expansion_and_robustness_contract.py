from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m246_b006_semantic_invariants_for_optimization_legality_edge_case_expansion_and_robustness_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m246_b006_semantic_invariants_for_optimization_legality_edge_case_expansion_and_robustness_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m246_b006_semantic_invariants_for_optimization_legality_edge_case_expansion_and_robustness_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_one(text: str, old: str, new: str) -> str:
    assert old in text
    return text.replace(old, new, 1)


def test_contract_parse_args_defaults() -> None:
    args = contract.parse_args([])
    assert args.b005_checker == contract.DEFAULT_B005_CHECKER
    assert args.b005_test == contract.DEFAULT_B005_TEST
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m246/M246-B006/")


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert (
        payload["mode"]
        == "m246-b006-semantic-invariants-optimization-legality-edge-case-expansion-and-robustness-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = (
        tmp_path / "m246_b006_semantic_invariants_for_optimization_legality_edge_case_expansion_and_robustness_packet.md"
    )
    drift_packet.write_text(
        replace_one(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#5065`",
            "Issue: `#5999`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-B006-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_chain_drops_b005_runner(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m246_b006_lane_b_readiness.py"
    drift_runner.write_text(
        replace_one(
            contract.DEFAULT_READINESS_RUNNER.read_text(encoding="utf-8"),
            "scripts/run_m246_b005_lane_b_readiness.py",
            "scripts/check_m246_b005_semantic_invariants_for_optimization_legality_edge_case_and_compatibility_completion_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M246-B006-RUN-02" for failure in payload["failures"])


def test_contract_summary_contract_for_failure_mode(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_b005_checker.py"
    exit_code = contract.run(
        [
            "--b005-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert set(payload) == {"mode", "ok", "checks_total", "checks_passed", "failures"}
    assert payload["ok"] is False
    assert payload["checks_passed"] < payload["checks_total"]
    assert isinstance(payload["failures"], list)
    assert payload["failures"]
    assert set(payload["failures"][0]) == {"artifact", "check_id", "detail"}
    assert any(failure["check_id"] == "M246-B006-DEP-B005-ARG-01" for failure in payload["failures"])

