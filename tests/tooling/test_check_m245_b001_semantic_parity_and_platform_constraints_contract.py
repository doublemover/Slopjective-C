from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = ROOT / "scripts" / "check_m245_b001_semantic_parity_and_platform_constraints_contract.py"
SPEC = importlib.util.spec_from_file_location(
    "check_m245_b001_semantic_parity_and_platform_constraints_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_m245_b001_semantic_parity_and_platform_constraints_contract.py")
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def test_contract_passes_on_repository_sources(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m245-b001-semantic-parity-platform-constraints-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 56
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m245_b001() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m245/M245-B001/")


def test_contract_fails_closed_when_contract_id_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m245_semantic_parity_and_platform_constraints_contract_and_architecture_freeze_b001_expectations.md"
    drift_doc.write_text(
        contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8").replace(
            "Contract ID: `objc3c-semantic-parity-platform-constraints-freeze/m245-b001-v1`",
            "Contract ID: `objc3c-semantic-parity-platform-constraints-freeze/m245-b001-drift`",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B001-DOC-EXP-02" for failure in payload["failures"])


def test_contract_fails_closed_when_typed_surface_runtime_dispatch_upper_bound_drifts(tmp_path: Path) -> None:
    drift_surface = tmp_path / "objc3_typed_sema_to_lowering_contract_surface.h"
    drift_surface.write_text(
        contract.DEFAULT_TYPED_SURFACE.read_text(encoding="utf-8").replace(
            "lowering_boundary.runtime_dispatch_arg_slots <= kObjc3RuntimeDispatchMaxArgs &&",
            "lowering_boundary.runtime_dispatch_arg_slots < kObjc3RuntimeDispatchMaxArgs &&",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--typed-surface", str(drift_surface), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B001-TYP-04" for failure in payload["failures"])


def test_contract_fails_closed_when_parse_readiness_forces_compatibility_true(tmp_path: Path) -> None:
    drift_readiness = tmp_path / "objc3_parse_lowering_readiness_surface.h"
    drift_readiness.write_text(
        contract.DEFAULT_PARSE_READINESS.read_text(encoding="utf-8")
        + "\n"
        + "surface.compatibility_handoff_consistent = true;\n",
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--parse-readiness", str(drift_readiness), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B001-PRS-FORB-01" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_lane_b_readiness_key_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m245_b001_semantic_parity_and_platform_constraints_contract_and_architecture_freeze_packet.md"
    drift_packet.write_text(
        contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8").replace(
            "check:objc3c:m245-b001-lane-b-readiness",
            "check:objc3c:m245-b001-lane-b-ready",
            1,
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M245-B001-DOC-PKT-08" for failure in payload["failures"])
