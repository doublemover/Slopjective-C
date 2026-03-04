from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m247_d002_runtime_link_build_throughput_optimization_modular_split_scaffolding_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_d002_runtime_link_build_throughput_optimization_modular_split_scaffolding_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_d002_runtime_link_build_throughput_optimization_modular_split_scaffolding_contract.py"
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
    assert payload["mode"] == "m247-d002-runtime-link-build-throughput-optimization-modular-split-scaffolding-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 40
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_d002() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-D002/")


def test_contract_summary_json_is_deterministic(tmp_path: Path) -> None:
    summary_out_a = tmp_path / "summary_a.json"
    summary_out_b = tmp_path / "summary_b.json"

    assert contract.run(["--summary-out", str(summary_out_a)]) == 0
    assert contract.run(["--summary-out", str(summary_out_b)]) == 0

    text_a = summary_out_a.read_text(encoding="utf-8")
    text_b = summary_out_b.read_text(encoding="utf-8")
    assert text_a == text_b
    payload = json.loads(text_a)
    assert text_a == contract.canonical_json(payload)


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_d002_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M247-D001`",
            "Dependencies: `M247-D099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D002-DOC-EXP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_d002_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#6760`",
            "Issue: `#6000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D002-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_drops_d001_chain(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m247_d002_lane_d_readiness.py"
    drift_readiness.write_text(
        replace_once(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "scripts/run_m247_d001_lane_d_readiness.py",
            "scripts/run_m247_d000_lane_d_readiness.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D002-RDY-02" for failure in payload["failures"])


def test_contract_fails_closed_when_d001_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_d001_expectations = tmp_path / "m247_d001_expectations.md"
    drift_d001_expectations.write_text(
        replace_once(
            contract.DEFAULT_D001_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: none\nScope: M247 lane-D runtime/link/build throughput optimization contract and architecture freeze for deterministic throughput governance continuity.",
            "Dependencies: `M247-D399`\nScope: M247 lane-D runtime/link/build throughput optimization contract and architecture freeze for deterministic throughput governance continuity.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--d001-expectations-doc",
            str(drift_d001_expectations),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D002-D001-DOC-02" for failure in payload["failures"])


def test_contract_fails_closed_when_d001_readiness_dependency_token_drifts(tmp_path: Path) -> None:
    drift_d001_readiness = tmp_path / "run_m247_d001_lane_d_readiness.py"
    drift_d001_readiness.write_text(
        replace_once(
            contract.DEFAULT_D001_READINESS_SCRIPT.read_text(encoding="utf-8"),
            'DEPENDENCY_TOKEN = "none"',
            'DEPENDENCY_TOKEN = "M247-D099"',
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--d001-readiness-script",
            str(drift_d001_readiness),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D002-D001-RDY-01" for failure in payload["failures"])


def test_contract_fails_closed_when_d001_checker_dependency_path_is_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--d001-checker",
            str(tmp_path / "missing_d001_checker.py"),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D002-DEP-D001-ARG-01" for failure in payload["failures"])
