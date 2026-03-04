from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m249_b011_semantic_compatibility_and_migration_checks_lane_b_integration_closeout_gate_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m249_b011_semantic_compatibility_and_migration_checks_lane_b_integration_closeout_gate_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m249_b011_semantic_compatibility_and_migration_checks_lane_b_integration_closeout_gate_contract.py"
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
        == "m249-b011-semantic-compatibility-migration-checks-lane-b-integration-closeout-gate-contract-v1"
    )
    assert payload["ok"] is True
    assert payload["checks_total"] >= 45
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m249_b011() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m249/M249-B011/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m249_b011_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M249-B010`",
            "Dependencies: `M249-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B011-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_expectations_drop_integration_closeout_wording(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m249_b011_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "integration closeout gate governance",
            "conformance corpus expansion governance",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B011-DOC-EXP-05" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_drops_mandatory_scope_wording(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m249_b011_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Code/spec anchors and milestone optimization improvements as mandatory scope inputs.",
            "Code/spec anchors and milestone optimization improvements as optional scope inputs.",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B011-DOC-PKT-09" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_no_longer_chains_b010(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m249_b011_lane_b_readiness.py"
    drift_runner.write_text(
        replace_all(
            contract.DEFAULT_READINESS_RUNNER.read_text(encoding="utf-8"),
            "scripts/run_m249_b010_lane_b_readiness.py",
            "scripts/check_m249_b010_semantic_compatibility_and_migration_checks_conformance_corpus_expansion_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B011-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_readiness_runner_drops_pytest_gate(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m249_b011_lane_b_readiness.py"
    drift_runner.write_text(
        replace_all(
            contract.DEFAULT_READINESS_RUNNER.read_text(encoding="utf-8"),
            "tests/tooling/test_check_m249_b011_semantic_compatibility_and_migration_checks_lane_b_integration_closeout_gate_contract.py",
            "tests/tooling/test_check_m249_b010_semantic_compatibility_and_migration_checks_conformance_corpus_expansion_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--readiness-runner", str(drift_runner), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B011-RUN-04" for failure in payload["failures"])


def test_contract_fails_closed_when_b010_expectations_contract_id_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m249_b010_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_B010_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Contract ID: `objc3c-semantic-compatibility-and-migration-checks-conformance-corpus-expansion/m249-b010-v1`",
            "Contract ID: `objc3c-semantic-compatibility-and-migration-checks-conformance-corpus-expansion/m249-b010-drift`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--b010-expectations-doc", str(drift_doc), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B011-B010-DOC-01" for failure in payload["failures"])


def test_contract_fails_closed_when_b010_packet_dependency_token_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m249_b010_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_B010_PACKET_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M249-B009`",
            "Dependencies: `M249-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(["--b010-packet-doc", str(drift_packet), "--summary-out", str(summary_out)])

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B011-B010-PKT-02" for failure in payload["failures"])


def test_contract_fails_closed_when_b010_readiness_runner_no_longer_chains_b009(tmp_path: Path) -> None:
    drift_runner = tmp_path / "run_m249_b010_lane_b_readiness.py"
    drift_runner.write_text(
        replace_all(
            contract.DEFAULT_B010_READINESS_RUNNER.read_text(encoding="utf-8"),
            "scripts/run_m249_b009_lane_b_readiness.py",
            "scripts/check_m249_b009_semantic_compatibility_and_migration_checks_conformance_matrix_implementation_contract.py",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            "--b010-readiness-runner",
            str(drift_runner),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B011-B010-RUN-02" for failure in payload["failures"])


def test_contract_fails_closed_when_b010_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_b010_checker.py"
    exit_code = contract.run(
        [
            "--b010-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B011-DEP-B010-ARG-01" for failure in payload["failures"])


def test_contract_fails_closed_when_b010_test_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_test = tmp_path / "missing_b010_test.py"
    exit_code = contract.run(
        [
            "--b010-test",
            str(missing_test),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B011-DEP-B010-ARG-02" for failure in payload["failures"])


def test_contract_fails_closed_when_b010_readiness_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_runner = tmp_path / "missing_b010_readiness.py"
    exit_code = contract.run(
        [
            "--b010-readiness-runner",
            str(missing_runner),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M249-B011-DEP-B010-ARG-03" for failure in payload["failures"])

