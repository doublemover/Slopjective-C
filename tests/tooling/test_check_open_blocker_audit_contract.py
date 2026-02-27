from __future__ import annotations

import importlib.util
import io
import itertools
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

import pytest

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "check_open_blocker_audit_contract.py"
FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "open_blocker_audit_contract"

EXPECTED_SUMMARY_KEY_ORDER = [
    "runner",
    "contract_id",
    "contract_version",
    "inputs",
    "scope",
    "artifacts",
    "audit",
    "commands",
    "errors",
    "final_status",
    "final_exit_code",
]
EXPECTED_SNAPSHOT_KEY_ORDER = [
    "contract_id",
    "contract_version",
    "generated_at_utc",
    "source",
    "open_blocker_count",
    "open_blockers",
]
EXPECTED_SNAPSHOT_ROW_KEY_ORDER = [
    "blocker_id",
    "source_path",
    "line_number",
    "line",
]
FAIL_CLOSED_SCENARIOS = (
    "hard_fail_summary_schema_drift",
    "hard_fail_snapshot_parity_key_order_drift",
    "hard_fail_log_contract_drift",
    "hard_fail_status_exit_mismatch",
)
SUMMARY_OPTION_CANDIDATES = ("--summary-json", "--summary", "--summary-path")
SNAPSHOT_OPTION_CANDIDATES = ("--snapshot-json", "--snapshot", "--snapshot-path")
EXTRACT_LOG_OPTION_CANDIDATES = ("--extract-log", "--log", "--extract-log-path", "--log-path")
EXPECTED_STATUS_OPTION_CANDIDATES = ("--expected-final-status", "--expected-status")
EXPECTED_EXIT_OPTION_CANDIDATES = ("--expected-final-exit-code", "--expected-exit-code")


if SCRIPT_PATH.exists():
    SPEC = importlib.util.spec_from_file_location("check_open_blocker_audit_contract", SCRIPT_PATH)
    if SPEC is None or SPEC.loader is None:
        raise RuntimeError("Unable to load scripts/check_open_blocker_audit_contract.py")
    check_open_blocker_audit_contract = importlib.util.module_from_spec(SPEC)
    sys.modules[SPEC.name] = check_open_blocker_audit_contract
    SPEC.loader.exec_module(check_open_blocker_audit_contract)
else:
    check_open_blocker_audit_contract = None


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def build_available_options() -> set[str]:
    if check_open_blocker_audit_contract is None:
        return set()

    parser_builder = getattr(check_open_blocker_audit_contract, "build_parser", None)
    if parser_builder is None:
        return set()

    parser = parser_builder()
    actions = getattr(parser, "_actions", ())
    available: set[str] = set()
    for action in actions:
        option_strings = getattr(action, "option_strings", ())
        for option in option_strings:
            available.add(option)
    return available


AVAILABLE_OPTIONS = build_available_options()


def append_required_option(
    args: list[str],
    *,
    label: str,
    candidates: tuple[str, ...],
    value: str,
) -> None:
    if AVAILABLE_OPTIONS:
        for option in candidates:
            if option in AVAILABLE_OPTIONS:
                args.extend([option, value])
                return
        raise AssertionError(
            f"checker parser missing required {label} option candidates {candidates!r}; "
            f"available={sorted(AVAILABLE_OPTIONS)!r}"
        )

    args.extend([candidates[0], value])


def append_optional_option(
    args: list[str],
    *,
    candidates: tuple[str, ...],
    value: str,
) -> None:
    if not AVAILABLE_OPTIONS:
        return
    for option in candidates:
        if option in AVAILABLE_OPTIONS:
            args.extend([option, value])
            return


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def scenario_artifact_paths(scenario_root: Path) -> tuple[Path, Path, Path]:
    return (
        scenario_root / "open_blocker_audit_summary.json",
        scenario_root / "inputs" / "open_blockers.snapshot.json",
        scenario_root / "extract_open_blockers.log",
    )


def scenario_args_with_flags(
    scenario_root: Path,
    *,
    summary_option: str,
    snapshot_option: str,
    extract_log_option: str,
) -> list[str]:
    summary_path, snapshot_path, extract_log_path = scenario_artifact_paths(scenario_root)
    return [
        summary_option,
        str(summary_path),
        snapshot_option,
        str(snapshot_path),
        extract_log_option,
        str(extract_log_path),
    ]


def extract_finding_check_ids(stderr: str) -> list[str]:
    check_ids: list[str] = []
    for line in stderr.splitlines():
        if not line.startswith("- [") or "] " not in line:
            continue
        check_ids.append(line[3 : line.index("] ")])
    return check_ids


def scenario_args(scenario_root: Path) -> list[str]:
    args: list[str] = []
    summary_path, snapshot_path, extract_log_path = scenario_artifact_paths(scenario_root)

    append_required_option(
        args,
        label="summary json",
        candidates=SUMMARY_OPTION_CANDIDATES,
        value=str(summary_path),
    )
    append_required_option(
        args,
        label="snapshot json",
        candidates=SNAPSHOT_OPTION_CANDIDATES,
        value=str(snapshot_path),
    )
    append_required_option(
        args,
        label="extract log",
        candidates=EXTRACT_LOG_OPTION_CANDIDATES,
        value=str(extract_log_path),
    )

    expected_status_path = scenario_root / "expected_final_status.txt"
    if expected_status_path.exists():
        append_optional_option(
            args,
            candidates=EXPECTED_STATUS_OPTION_CANDIDATES,
            value=read_text(expected_status_path).strip(),
        )

    expected_exit_path = scenario_root / "expected_final_exit_code.txt"
    if expected_exit_path.exists():
        append_optional_option(
            args,
            candidates=EXPECTED_EXIT_OPTION_CANDIDATES,
            value=read_text(expected_exit_path).strip(),
        )

    return args


def run_main(args: list[str]) -> tuple[int, str, str]:
    if check_open_blocker_audit_contract is None:
        raise AssertionError("checker module is unavailable")

    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = check_open_blocker_audit_contract.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def run_scenario(name: str) -> tuple[int, str, str]:
    return run_main(scenario_args(FIXTURE_ROOT / name))


def assert_stderr_matches_fixture(stderr: str, expected_stderr_path: Path) -> None:
    expected_lines = [line for line in read_text(expected_stderr_path).splitlines() if line]
    assert expected_lines, f"expected stderr fixture must include non-empty line(s): {expected_stderr_path}"

    cursor = 0
    for line in expected_lines:
        index = stderr.find(line, cursor)
        assert index >= 0, f"missing expected stderr line {line!r} in output: {stderr!r}"
        cursor = index + len(line)


def test_happy_fixture_contract_fields_and_key_order_are_canonical() -> None:
    scenario_root = FIXTURE_ROOT / "happy"
    summary = read_json(scenario_root / "open_blocker_audit_summary.json")
    snapshot = read_json(scenario_root / "inputs" / "open_blockers.snapshot.json")

    assert list(summary.keys()) == EXPECTED_SUMMARY_KEY_ORDER
    assert summary["contract_id"] == "open-blocker-audit-runner"
    assert summary["contract_version"] == "v0.1"

    assert list(snapshot.keys()) == EXPECTED_SNAPSHOT_KEY_ORDER
    assert snapshot["contract_id"] == "open-blocker-audit-runner"
    assert snapshot["contract_version"] == "v0.1"
    assert snapshot["open_blocker_count"] == 0
    assert snapshot["open_blockers"] == []


def test_summary_schema_drift_fixture_breaks_contract_field_order() -> None:
    scenario_root = FIXTURE_ROOT / "hard_fail_summary_schema_drift"
    summary = read_json(scenario_root / "open_blocker_audit_summary.json")

    observed_key_order = list(summary.keys())
    assert observed_key_order != EXPECTED_SUMMARY_KEY_ORDER
    assert "contract_version" in observed_key_order
    assert observed_key_order.index("contract_version") > observed_key_order.index("final_exit_code")
    assert summary["contract_id"] == "open-blocker-audit-runner"
    assert summary["contract_version"] == "v0.9-drift"


def test_snapshot_parity_key_order_drift_fixture_breaks_contract_fields() -> None:
    scenario_root = FIXTURE_ROOT / "hard_fail_snapshot_parity_key_order_drift"
    snapshot = read_json(scenario_root / "inputs" / "open_blockers.snapshot.json")

    observed_key_order = list(snapshot.keys())
    assert observed_key_order != EXPECTED_SNAPSHOT_KEY_ORDER
    assert observed_key_order[:2] == ["contract_version", "contract_id"]

    rows = snapshot["open_blockers"]
    assert isinstance(rows, list)
    assert len(rows) == 1
    row = rows[0]
    assert isinstance(row, dict)
    assert list(row.keys()) != EXPECTED_SNAPSHOT_ROW_KEY_ORDER

    assert snapshot["open_blocker_count"] == 0
    assert len(rows) == 1


def test_fail_closed_fixture_matrix_has_expected_stderr_and_nonzero_exit() -> None:
    for scenario_name in FAIL_CLOSED_SCENARIOS:
        scenario_root = FIXTURE_ROOT / scenario_name
        expected_exit_code = int(read_text(scenario_root / "expected_exit_code.txt").strip())
        expected_stderr = read_text(scenario_root / "expected_stderr.txt")

        assert expected_exit_code != 0
        assert expected_stderr.strip() != ""


@pytest.mark.skipif(
    check_open_blocker_audit_contract is None,
    reason="scripts/check_open_blocker_audit_contract.py is not present in this workspace snapshot",
)
def test_required_flag_alias_matrix_matches_happy_result() -> None:
    scenario_root = FIXTURE_ROOT / "happy"
    candidate_matrix = (
        ("summary json", SUMMARY_OPTION_CANDIDATES),
        ("snapshot json", SNAPSHOT_OPTION_CANDIDATES),
        ("extract log", EXTRACT_LOG_OPTION_CANDIDATES),
    )

    supported_candidates: list[list[str]] = []
    for label, candidates in candidate_matrix:
        resolved = [option for option in candidates if option in AVAILABLE_OPTIONS]
        assert resolved, (
            f"checker parser missing required {label} option candidates {candidates!r}; "
            f"available={sorted(AVAILABLE_OPTIONS)!r}"
        )
        supported_candidates.append(resolved)

    baseline_args = scenario_args_with_flags(
        scenario_root,
        summary_option=supported_candidates[0][0],
        snapshot_option=supported_candidates[1][0],
        extract_log_option=supported_candidates[2][0],
    )
    baseline_result = run_main(baseline_args)
    assert baseline_result[0] == 0
    assert baseline_result[2] == ""

    for summary_option, snapshot_option, extract_log_option in itertools.product(*supported_candidates):
        args = scenario_args_with_flags(
            scenario_root,
            summary_option=summary_option,
            snapshot_option=snapshot_option,
            extract_log_option=extract_log_option,
        )
        assert run_main(args) == baseline_result


@pytest.mark.skipif(
    check_open_blocker_audit_contract is None,
    reason="scripts/check_open_blocker_audit_contract.py is not present in this workspace snapshot",
)
def test_happy_path_is_deterministic() -> None:
    scenario_root = FIXTURE_ROOT / "happy"
    args = scenario_args(scenario_root)
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == ""
    assert second_stderr == ""
    assert first_stdout == second_stdout
    assert first_stdout.endswith("\n")

    summary_path, snapshot_path, extract_log_path = scenario_artifact_paths(scenario_root)
    expected_payload = {
        "mode": check_open_blocker_audit_contract.MODE,
        "contract": {
            "expected_runner": (
                f"{check_open_blocker_audit_contract.DEFAULT_CONTRACT_ID}/"
                f"{check_open_blocker_audit_contract.DEFAULT_CONTRACT_VERSION}"
            ),
            "contract_id": check_open_blocker_audit_contract.DEFAULT_CONTRACT_ID,
            "contract_version": check_open_blocker_audit_contract.DEFAULT_CONTRACT_VERSION,
        },
        "artifacts": {
            "summary": check_open_blocker_audit_contract.display_path(summary_path),
            "snapshot": check_open_blocker_audit_contract.display_path(snapshot_path),
            "extract_log": check_open_blocker_audit_contract.display_path(extract_log_path),
        },
        "ok": True,
        "exit_code": 0,
        "finding_count": 0,
        "findings": [],
    }
    assert json.loads(first_stdout) == expected_payload
    assert first_stdout == json.dumps(expected_payload, indent=2) + "\n"


@pytest.mark.skipif(
    check_open_blocker_audit_contract is None,
    reason="scripts/check_open_blocker_audit_contract.py is not present in this workspace snapshot",
)
@pytest.mark.parametrize("scenario_name", FAIL_CLOSED_SCENARIOS)
def test_fixture_backed_fail_closed_paths(scenario_name: str) -> None:
    scenario_root = FIXTURE_ROOT / scenario_name
    expected_exit_code = int(read_text(scenario_root / "expected_exit_code.txt").strip())
    expected_stderr_path = scenario_root / "expected_stderr.txt"

    code, stdout, stderr = run_scenario(scenario_name)

    assert code == expected_exit_code
    assert stdout == ""
    assert_stderr_matches_fixture(stderr, expected_stderr_path)


@pytest.mark.skipif(
    check_open_blocker_audit_contract is None,
    reason="scripts/check_open_blocker_audit_contract.py is not present in this workspace snapshot",
)
@pytest.mark.parametrize("scenario_name", FAIL_CLOSED_SCENARIOS)
def test_fail_closed_diagnostics_transcript_is_deterministic_and_sorted(scenario_name: str) -> None:
    scenario_root = FIXTURE_ROOT / scenario_name
    expected_exit_code = int(read_text(scenario_root / "expected_exit_code.txt").strip())
    expected_stderr_path = scenario_root / "expected_stderr.txt"
    args = scenario_args(scenario_root)
    first_code, first_stdout, first_stderr = run_main(args)
    second_code, second_stdout, second_stderr = run_main(args)

    assert (first_code, first_stdout, first_stderr) == (second_code, second_stdout, second_stderr)
    assert first_code == expected_exit_code
    assert first_stdout == ""
    assert first_stderr.startswith("open-blocker-audit-contract: contract drift detected (")
    assert "\ndrift findings:\n" in first_stderr
    assert first_stderr.endswith("\n")
    assert_stderr_matches_fixture(first_stderr, expected_stderr_path)

    summary_path, snapshot_path, extract_log_path = scenario_artifact_paths(scenario_root)
    findings = check_open_blocker_audit_contract.validate_contract(
        summary_path=summary_path,
        snapshot_path=snapshot_path,
        extract_log_path=extract_log_path,
        contract_id=check_open_blocker_audit_contract.DEFAULT_CONTRACT_ID,
        contract_version=check_open_blocker_audit_contract.DEFAULT_CONTRACT_VERSION,
    )
    check_ids = [finding["check_id"] for finding in findings]
    assert check_ids == sorted(check_ids)
    assert check_ids == extract_finding_check_ids(first_stderr)

    expected_rendered = check_open_blocker_audit_contract.render_fail_closed_diagnostics(
        findings,
        extract_log_path=extract_log_path,
    )
    assert first_stderr == expected_rendered
