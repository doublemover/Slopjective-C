from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m247_d001_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m247_d001_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m247_d001_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract.py"
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
    assert payload["mode"] == (
        "m247-d001-runtime-link-build-throughput-optimization-contract-and-architecture-freeze-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 35
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m247_d001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m247/M247-D001/")


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


def test_contract_fails_closed_when_expectations_issue_anchor_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m247_d001_expectations.md"
    drift_doc.write_text(
        replace_once(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Issue `#6759` defines canonical lane-D contract and architecture freeze scope.",
            "Issue `#6000` defines canonical lane-D contract and architecture freeze scope.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D001-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_dependency_token_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m247_d001_packet.md"
    drift_packet.write_text(
        replace_once(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Dependencies: none",
            "Dependencies: `M247-D999`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D001-DOC-PKT-04" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_script_drops_pytest_gate(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "run_m247_d001_lane_d_readiness.py"
    drift_readiness.write_text(
        replace_once(
            contract.DEFAULT_READINESS_SCRIPT.read_text(encoding="utf-8"),
            "tests/tooling/test_check_m247_d001_runtime_link_build_throughput_optimization_contract_and_architecture_freeze_contract.py",
            "tests/tooling/missing_contract_test.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-script", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D001-RDY-03" for failure in payload["failures"])


def test_contract_fails_closed_when_architecture_dependency_anchor_drifts(tmp_path: Path) -> None:
    drift_architecture = tmp_path / "ARCHITECTURE.md"
    drift_architecture.write_text(
        replace_once(
            contract.DEFAULT_ARCHITECTURE_DOC.read_text(encoding="utf-8"),
            "`M247-A001`, `M247-B001`, `M247-C001`, `M247-D001`",
            "`M247-A001`, `M247-B001`, `M247-C001`, `M247-D099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--architecture-doc", str(drift_architecture), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D001-ARCH-02" for failure in payload["failures"])


def test_contract_fails_closed_when_metadata_pending_lane_anchor_drifts(tmp_path: Path) -> None:
    drift_metadata = tmp_path / "MODULE_METADATA_AND_ABI_TABLES.md"
    drift_metadata.write_text(
        replace_once(
            contract.DEFAULT_METADATA_SPEC.read_text(encoding="utf-8"),
            "`M247-C001`, and `M247-D001`, including pending-lane tokens needed",
            "`M247-C001`, and `M247-D099`, including pending-lane tokens needed",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--metadata-spec", str(drift_metadata), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M247-D001-META-02" for failure in payload["failures"])
