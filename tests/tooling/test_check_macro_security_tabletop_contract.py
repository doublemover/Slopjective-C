from __future__ import annotations

import importlib.util
import io
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "check_macro_security_tabletop_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_macro_security_tabletop_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_macro_security_tabletop_contract.py")
module = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = module
SPEC.loader.exec_module(module)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "macro_security_tabletop_contract"
HAPPY_ROOT = FIXTURE_ROOT / "happy"
DRIFT_REPORT_PATH = FIXTURE_ROOT / "drift_missing_scenario_summary" / "report.md"
W2_DRIFT_PACKAGE_PATH = FIXTURE_ROOT / "w2_drift_missing_m11_addendum" / "package.md"
W2_INVALID_UTF8_REPORT_PATH = FIXTURE_ROOT / "w2_hard_fail_invalid_utf8" / "report.md"


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
        "--playbook",
        str(HAPPY_ROOT / "playbook.md"),
        "--report",
        str(HAPPY_ROOT / "report.md"),
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == ""
    assert second_stderr == ""
    assert first_stdout == second_stdout
    assert first_stdout == (
        "macro-security-tabletop-contract: OK\n"
        "- mode=macro-security-tabletop-contract-v2\n"
        "- package=tests/tooling/fixtures/macro_security_tabletop_contract/happy/package.md\n"
        "- playbook=tests/tooling/fixtures/macro_security_tabletop_contract/happy/playbook.md\n"
        "- report=tests/tooling/fixtures/macro_security_tabletop_contract/happy/report.md\n"
        "- checks_passed=33\n"
        "- fail_closed=true\n"
    )


def test_drift_path_returns_exit_1_with_stable_diagnostics() -> None:
    code, stdout, stderr = run_main(
        [
            "--package",
            str(HAPPY_ROOT / "package.md"),
            "--playbook",
            str(HAPPY_ROOT / "playbook.md"),
            "--report",
            str(DRIFT_REPORT_PATH),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert stderr == (
        "macro-security-tabletop-contract: contract drift detected (1 failed check(s)).\n"
        "drift findings:\n"
        "- report:RPT-05\n"
        "  expected snippet: Scenario matrix summary: `5/5` scenarios passed severity/tier "
        "determinism checks.\n"
        "remediation:\n"
        "1. Restore missing contract snippet(s) in the listed artifact(s).\n"
        "2. Re-run validator:\n"
        "python scripts/check_macro_security_tabletop_contract.py --package "
        "tests/tooling/fixtures/macro_security_tabletop_contract/happy/package.md "
        "--playbook tests/tooling/fixtures/macro_security_tabletop_contract/happy/playbook.md "
        "--report tests/tooling/fixtures/macro_security_tabletop_contract/"
        "drift_missing_scenario_summary/report.md\n"
    )


def test_w2_drift_path_orders_multi_finding_diagnostics() -> None:
    code, stdout, stderr = run_main(
        [
            "--package",
            str(W2_DRIFT_PACKAGE_PATH),
            "--playbook",
            str(HAPPY_ROOT / "playbook.md"),
            "--report",
            str(HAPPY_ROOT / "report.md"),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert stderr == (
        "macro-security-tabletop-contract: contract drift detected (4 failed check(s)).\n"
        "drift findings:\n"
        "- package:PKG-09\n"
        "  expected snippet: ## 10. M11 Macro Security Tabletop Ops W1 Addendum "
        "(`MSTP-DEP-M11-*`)\n"
        "- package:PKG-10\n"
        "  expected snippet: | `MSTP-DEP-M11-06` | `Soft` | Open remediation drift is advisory "
        "`HOLD` only when rows retain explicit owner, due date, and evidence hook "
        "while all hard controls remain intact. |\n"
        "- package:PKG-11\n"
        "  expected snippet: | `MSTP-DEP-M11-07` | `Hard` | M11 disposition semantics are "
        "fail-closed with explicit `PASS`/`HOLD`/`FAIL` rules and no waiver path "
        "for hard failures. |\n"
        "- package:PKG-12\n"
        "  expected snippet: 4. Fail-closed default: if evidence is ambiguous, missing, or "
        "contradictory, disposition is `FAIL` until deterministic alignment is "
        "restored.\n"
        "remediation:\n"
        "1. Restore missing contract snippet(s) in the listed artifact(s).\n"
        "2. Re-run validator:\n"
        "python scripts/check_macro_security_tabletop_contract.py --package "
        "tests/tooling/fixtures/macro_security_tabletop_contract/"
        "w2_drift_missing_m11_addendum/package.md --playbook "
        "tests/tooling/fixtures/macro_security_tabletop_contract/happy/playbook.md "
        "--report tests/tooling/fixtures/macro_security_tabletop_contract/happy/report.md\n"
    )


def test_hard_fail_returns_exit_2_for_missing_file() -> None:
    code, stdout, stderr = run_main(
        [
            "--package",
            str(HAPPY_ROOT / "package.md"),
            "--playbook",
            str(HAPPY_ROOT / "playbook.md"),
            "--report",
            str(FIXTURE_ROOT / "drift_missing_scenario_summary" / "missing_report.md"),
        ]
    )

    assert code == 2
    assert stdout == ""
    assert stderr == (
        "macro-security-tabletop-contract: error: report file does not exist: "
        "tests/tooling/fixtures/macro_security_tabletop_contract/"
        "drift_missing_scenario_summary/missing_report.md\n"
    )


def test_w2_hard_fail_returns_exit_2_for_invalid_utf8() -> None:
    code, stdout, stderr = run_main(
        [
            "--package",
            str(HAPPY_ROOT / "package.md"),
            "--playbook",
            str(HAPPY_ROOT / "playbook.md"),
            "--report",
            str(W2_INVALID_UTF8_REPORT_PATH),
        ]
    )

    assert code == 2
    assert stdout == ""
    assert stderr == (
        "macro-security-tabletop-contract: error: report file is not valid UTF-8: "
        "tests/tooling/fixtures/macro_security_tabletop_contract/"
        "w2_hard_fail_invalid_utf8/report.md\n"
    )
