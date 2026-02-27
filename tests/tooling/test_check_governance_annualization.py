from __future__ import annotations

import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "check_governance_annualization.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_governance_annualization",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_governance_annualization.py for tests.")
check_governance_annualization = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = check_governance_annualization
SPEC.loader.exec_module(check_governance_annualization)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "governance_annualization"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = check_governance_annualization.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def test_baseline_json_happy_path_and_deterministic_order() -> None:
    args = [
        "--input",
        str(FIXTURE_ROOT / "baseline_ok.json"),
        "--format",
        "json",
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == ""
    assert second_stderr == ""
    assert first_stdout == second_stdout

    payload = json.loads(first_stdout)
    assert isinstance(payload, dict)
    assert list(payload.keys()) == [
        "mode",
        "input",
        "required_fields",
        "annual_review",
        "cadence",
        "quorum",
        "publication_windows",
        "exceptions",
        "escalation_records",
        "checks",
        "failures",
        "result",
        "exit_code",
    ]
    assert payload["mode"] == "governance-annualization-deterministic"
    assert payload["required_fields"] == [
        "annual_review_due_utc",
        "annual_review_completed_utc",
        "cadence.utc_anchor",
        "quorum.present_voters",
        "quorum.vendor_count",
        "quorum.spec_editor_present",
        "quorum.tooling_owner_present",
        "quorum.recusal_ratio",
        "publication_windows.attestation_deadline_utc",
        "publication_windows.minutes_deadline_utc",
        "publication_windows.packet_deadline_utc",
        "exceptions",
        "exceptions[].exception_id",
        "exceptions[].status",
        "exceptions[].opened_utc",
        "exceptions[].expires_utc",
        "exceptions[].renewed_utc",
        "exceptions[].renewal_expires_utc",
        "escalation_records",
        "escalation_records[].escalation_id",
        "escalation_records[].linked_exception_id",
        "escalation_records[].owner",
        "escalation_records[].opened_utc",
        "escalation_records[].response_due_utc",
        "escalation_records[].status",
        "escalation_records[].closed_utc",
    ]
    checks = payload["checks"]
    assert isinstance(checks, dict)
    assert checks == {
        "annual_review_pass": True,
        "quorum_pass": True,
        "publication_chronology_pass": True,
        "publication_deadline_regression_pass": True,
        "exception_renewal_pass": True,
        "exception_escalation_linkage_pass": True,
        "escalation_owner_pass": True,
        "escalation_contract_pass": True,
    }
    exceptions = payload["exceptions"]
    assert isinstance(exceptions, dict)
    assert exceptions["count"] == 0
    escalation_records = payload["escalation_records"]
    assert isinstance(escalation_records, dict)
    assert escalation_records["count"] == 0
    quorum = payload["quorum"]
    assert isinstance(quorum, dict)
    predicates = quorum["predicates"]
    assert isinstance(predicates, list)
    assert [row["id"] for row in predicates] == ["Q-01", "Q-02", "Q-03", "Q-04"]
    assert payload["failures"] == []
    assert payload["result"] == "PASS"
    assert payload["exit_code"] == 0


def test_quorum_fail_returns_contract_mismatch_exit_1() -> None:
    code, stdout, stderr = run_main(
        [
            "--input",
            str(FIXTURE_ROOT / "quorum_fail.json"),
            "--format",
            "json",
        ]
    )

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    checks = payload["checks"]
    assert isinstance(checks, dict)
    assert checks["annual_review_pass"] is True
    assert checks["quorum_pass"] is False
    assert checks["publication_chronology_pass"] is True
    assert checks["publication_deadline_regression_pass"] is True
    assert checks["exception_renewal_pass"] is True
    assert checks["exception_escalation_linkage_pass"] is True
    assert checks["escalation_owner_pass"] is True
    assert checks["escalation_contract_pass"] is True

    failures = payload["failures"]
    assert isinstance(failures, list)
    assert [entry["code"] for entry in failures] == [
        "quorum_q01_fail",
        "quorum_q02_fail",
        "quorum_q03_fail",
        "quorum_q04_fail",
    ]
    assert payload["result"] == "FAIL"
    assert payload["exit_code"] == 1


def test_missing_field_returns_shape_error_exit_2() -> None:
    code, stdout, stderr = run_main(
        [
            "--input",
            str(FIXTURE_ROOT / "missing_field.json"),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "annual_review_completed_utc" in stderr
    assert "must be a non-empty string" in stderr


def test_markdown_output_mode_is_deterministic() -> None:
    args = [
        "--input",
        str(FIXTURE_ROOT / "baseline_ok.json"),
        "--format",
        "markdown",
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == ""
    assert second_stderr == ""
    assert first_stdout == second_stdout

    assert first_stdout.startswith("# Governance Annualization Check\n")
    assert "- Result: `PASS`" in first_stdout
    assert "- Exit code: `0`" in first_stdout
    assert "| `Q-01` | present_voters >= 5 | `true` |" in first_stdout
    assert (
        "| `cadence.utc_anchor < publication_windows.attestation_deadline_utc` | `true` |"
        in first_stdout
    )
    assert "- publication_deadline_regression_pass: `true`" in first_stdout
    assert "- exception_renewal_pass: `true`" in first_stdout
    assert "- escalation_owner_pass: `true`" in first_stdout
    assert "## Failures" in first_stdout
    assert "- _none_" in first_stdout


def test_w2_escalation_owner_completeness_fail_returns_exit_1() -> None:
    code, stdout, stderr = run_main(
        [
            "--input",
            str(FIXTURE_ROOT / "w2_escalation_missing_owner.json"),
            "--format",
            "json",
        ]
    )

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    checks = payload["checks"]
    assert isinstance(checks, dict)
    assert checks["escalation_owner_pass"] is False
    assert checks["escalation_contract_pass"] is False

    failures = payload["failures"]
    assert isinstance(failures, list)
    assert [entry["code"] for entry in failures] == ["escalation_owner_incomplete"]


def test_w2_exception_expiry_renewal_and_escalation_linkage_fail_exit_1() -> None:
    code, stdout, stderr = run_main(
        [
            "--input",
            str(FIXTURE_ROOT / "w2_exception_expired.json"),
            "--format",
            "json",
        ]
    )

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    checks = payload["checks"]
    assert isinstance(checks, dict)
    assert checks["exception_renewal_pass"] is False
    assert checks["exception_escalation_linkage_pass"] is False

    failures = payload["failures"]
    assert isinstance(failures, list)
    assert [entry["code"] for entry in failures] == [
        "exception_state_expired_without_renewal",
        "exception_expiry_escalation_missing",
    ]


def test_w2_publication_deadline_regression_returns_exit_1() -> None:
    code, stdout, stderr = run_main(
        [
            "--input",
            str(FIXTURE_ROOT / "w2_deadline_regression.json"),
            "--format",
            "json",
        ]
    )

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    checks = payload["checks"]
    assert isinstance(checks, dict)
    assert checks["publication_chronology_pass"] is True
    assert checks["publication_deadline_regression_pass"] is False

    failures = payload["failures"]
    assert isinstance(failures, list)
    assert [entry["code"] for entry in failures] == [
        "publication_attestation_deadline_regression",
        "publication_minutes_deadline_regression",
        "publication_packet_deadline_regression",
    ]
