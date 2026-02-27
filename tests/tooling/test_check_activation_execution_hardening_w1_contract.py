from __future__ import annotations

import importlib.util
import io
import shutil
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2]
    / "scripts"
    / "check_activation_execution_hardening_w1_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_activation_execution_hardening_w1_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_activation_execution_hardening_w1_contract.py"
    )
module = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = module
SPEC.loader.exec_module(module)

CHECKER_SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "check_activation_triggers.py"
FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "activation_triggers"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = module.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def test_happy_path_is_deterministic() -> None:
    args = [
        "--checker-script",
        str(CHECKER_SCRIPT_PATH),
        "--fixture-root",
        str(FIXTURE_ROOT),
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == ""
    assert second_stderr == ""
    assert first_stdout == second_stdout
    checks_passed = (
        len(module.discover_happy_scenarios(fixture_root=FIXTURE_ROOT))
        + len(module.SCHEMA_CHECK_IDS)
        + len(module.discover_fail_closed_scenarios(fixture_root=FIXTURE_ROOT))
    )
    assert first_stdout == (
        "activation-execution-hardening-w1-contract: OK\n"
        "- mode=activation-execution-hardening-w1-contract-v1\n"
        "- checker_script=scripts/check_activation_triggers.py\n"
        "- fixture_root=tests/tooling/fixtures/activation_triggers\n"
        f"- checks_passed={checks_passed}\n"
        "- fail_closed=true\n"
    )


def test_drift_path_returns_exit_1_with_stable_report(tmp_path: Path) -> None:
    fixture_root = tmp_path / "activation_triggers"
    shutil.copytree(FIXTURE_ROOT, fixture_root)
    (fixture_root / "zero_open" / "expected.json").write_text(
        "{\"drift\": true}\n",
        encoding="utf-8",
    )
    args = [
        "--checker-script",
        str(CHECKER_SCRIPT_PATH),
        "--fixture-root",
        str(fixture_root),
    ]
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 1
    assert second_code == 1
    assert first_stdout == ""
    assert second_stdout == ""
    assert first_stderr == second_stderr
    assert "activation-execution-hardening-w1-contract: contract drift detected" in first_stderr
    assert "- W1C-01\n" in first_stderr
    assert "zero_open/json output drift against" in first_stderr
    assert "remediation:\n" in first_stderr
    assert "python scripts/check_activation_execution_hardening_w1_contract.py" in first_stderr


def test_hard_fail_returns_exit_2_for_missing_checker_script() -> None:
    missing_checker_path = FIXTURE_ROOT / "missing_check_activation_triggers.py"
    code, stdout, stderr = run_main(
        [
            "--checker-script",
            str(missing_checker_path),
            "--fixture-root",
            str(FIXTURE_ROOT),
        ]
    )

    assert code == 2
    assert stdout == ""
    assert (
        "activation-execution-hardening-w1-contract: error: checker script does not exist:"
        in stderr
    )
