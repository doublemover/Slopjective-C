from __future__ import annotations

import importlib.util
import io
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "check_abstract_machine_sync_audit_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_abstract_machine_sync_audit_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_abstract_machine_sync_audit_contract.py")
module = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = module
SPEC.loader.exec_module(module)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "abstract_machine_sync_audit_contract"
HAPPY_ROOT = FIXTURE_ROOT / "happy"
DRIFT_REPORT_PATH = FIXTURE_ROOT / "drift_missing_tie_break" / "report.md"
W2_DRIFT_PACKAGE_PATH = FIXTURE_ROOT / "w2_drift_missing_hardening_semantics" / "package.md"
W2_INVALID_UTF8_SIGNOFF_PATH = FIXTURE_ROOT / "w2_hard_fail_invalid_utf8" / "signoff.md"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = module.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def test_happy_path_is_deterministic() -> None:
    args = [
        "--package",
        str(HAPPY_ROOT / "package.md"),
        "--report",
        str(HAPPY_ROOT / "report.md"),
        "--signoff",
        str(HAPPY_ROOT / "signoff.md"),
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == ""
    assert second_stderr == ""
    assert first_stdout == second_stdout
    assert first_stdout == (
        "abstract-machine-sync-audit-contract: OK\n"
        "- mode=abstract-machine-sync-audit-contract-v2\n"
        "- package=tests/tooling/fixtures/abstract_machine_sync_audit_contract/happy/package.md\n"
        "- report=tests/tooling/fixtures/abstract_machine_sync_audit_contract/happy/report.md\n"
        "- signoff=tests/tooling/fixtures/abstract_machine_sync_audit_contract/happy/signoff.md\n"
        "- checks_passed=31\n"
        "- fail_closed=true\n"
    )


def test_drift_path_returns_exit_1_with_stable_diagnostics() -> None:
    code, stdout, stderr = run_main(
        [
            "--package",
            str(HAPPY_ROOT / "package.md"),
            "--report",
            str(DRIFT_REPORT_PATH),
            "--signoff",
            str(HAPPY_ROOT / "signoff.md"),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert stderr == (
        "abstract-machine-sync-audit-contract: contract drift detected (1 failed check(s)).\n"
        "drift findings:\n"
        "- report:RPT-10\n"
        "  expected snippet: Tie-break rule: if scores are equal, sort by lexical `drift_id`.\n"
        "remediation:\n"
        "1. Restore missing contract snippet(s) in the listed artifact(s).\n"
        "2. Re-run validator:\n"
        "python scripts/check_abstract_machine_sync_audit_contract.py --package "
        "tests/tooling/fixtures/abstract_machine_sync_audit_contract/happy/package.md "
        "--report tests/tooling/fixtures/abstract_machine_sync_audit_contract/"
        "drift_missing_tie_break/report.md --signoff "
        "tests/tooling/fixtures/abstract_machine_sync_audit_contract/happy/signoff.md\n"
    )


def test_w2_drift_path_orders_multi_finding_diagnostics_deterministically() -> None:
    args = [
        "--package",
        str(W2_DRIFT_PACKAGE_PATH),
        "--report",
        str(HAPPY_ROOT / "report.md"),
        "--signoff",
        str(HAPPY_ROOT / "signoff.md"),
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 1
    assert second_code == 1
    assert first_stdout == ""
    assert second_stdout == ""
    assert first_stderr == second_stderr
    assert first_stderr == (
        "abstract-machine-sync-audit-contract: contract drift detected (4 failed check(s)).\n"
        "drift findings:\n"
        "- package:PKG-07\n"
        "  expected snippet: | `AMSA-DEP-M13-05` | `Hard` | Every M13 acceptance row must "
        "declare dependency type, fail criteria, escalation owner, and unblock "
        "condition. |\n"
        "- package:PKG-12\n"
        "  expected snippet: 2. `HOLD`: only soft row `AMSA-DEP-M13-06` remains open "
        "with explicit owner, ETA, and bounded-risk note while all hard rows "
        "remain `PASS`.\n"
        "- package:PKG-13\n"
        "  expected snippet: 3. `FAIL`: any hard row fails, required evidence mapping "
        "is missing, or soft-row drift is used to bypass hard controls.\n"
        "- package:PKG-14\n"
        "  expected snippet: 4. Fail-closed default: if evidence is ambiguous, missing, "
        "or contradictory, disposition is `FAIL` until deterministic alignment "
        "is restored.\n"
        "remediation:\n"
        "1. Restore missing contract snippet(s) in the listed artifact(s).\n"
        "2. Re-run validator:\n"
        "python scripts/check_abstract_machine_sync_audit_contract.py --package "
        "tests/tooling/fixtures/abstract_machine_sync_audit_contract/"
        "w2_drift_missing_hardening_semantics/package.md --report "
        "tests/tooling/fixtures/abstract_machine_sync_audit_contract/happy/report.md "
        "--signoff tests/tooling/fixtures/abstract_machine_sync_audit_contract/"
        "happy/signoff.md\n"
    )


def test_hard_fail_returns_exit_2_for_missing_file() -> None:
    code, stdout, stderr = run_main(
        [
            "--package",
            str(HAPPY_ROOT / "package.md"),
            "--report",
            str(FIXTURE_ROOT / "drift_missing_tie_break" / "missing_report.md"),
            "--signoff",
            str(HAPPY_ROOT / "signoff.md"),
        ]
    )

    assert code == 2
    assert stdout == ""
    assert stderr == (
        "abstract-machine-sync-audit-contract: error: report file does not exist: "
        "tests/tooling/fixtures/abstract_machine_sync_audit_contract/"
        "drift_missing_tie_break/missing_report.md\n"
    )


def test_w2_hard_fail_returns_exit_2_for_invalid_utf8_signoff() -> None:
    code, stdout, stderr = run_main(
        [
            "--package",
            str(HAPPY_ROOT / "package.md"),
            "--report",
            str(HAPPY_ROOT / "report.md"),
            "--signoff",
            str(W2_INVALID_UTF8_SIGNOFF_PATH),
        ]
    )

    assert code == 2
    assert stdout == ""
    assert stderr == (
        "abstract-machine-sync-audit-contract: error: signoff file is not valid UTF-8: "
        "tests/tooling/fixtures/abstract_machine_sync_audit_contract/"
        "w2_hard_fail_invalid_utf8/signoff.md\n"
    )
