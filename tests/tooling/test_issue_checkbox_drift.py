import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "check_issue_checkbox_drift.py"
SPEC = importlib.util.spec_from_file_location("check_issue_checkbox_drift", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_issue_checkbox_drift.py for tests.")
check_issue_checkbox_drift = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = check_issue_checkbox_drift
SPEC.loader.exec_module(check_issue_checkbox_drift)

ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures"
ISSUES_SNAPSHOT = FIXTURE_ROOT / "issues_snapshot.json"
ISSUE_DRIFT_FIXTURES = FIXTURE_ROOT / "issue_drift"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = check_issue_checkbox_drift.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def fixture_glob(name: str) -> str:
    return (ISSUE_DRIFT_FIXTURES / name).relative_to(ROOT).as_posix()


def test_offline_snapshot_with_one_mismatch_exits_1() -> None:
    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(ISSUES_SNAPSHOT),
            "--glob",
            fixture_glob("offline_mismatch.md"),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "issue-drift: FAIL" in stderr
    assert "found 1 mismatch(es);" in stderr
    assert "drift threshold: 0;" in stderr
    assert "overlap count: 0;" in stderr
    assert "max overlap count: 0;" in stderr
    assert "max verdict: PASS;" in stderr
    assert "threshold gate: blocked;" in stderr
    assert "checked row count: 1" in stderr


def test_offline_snapshot_with_aligned_rows_exits_0() -> None:
    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(ISSUES_SNAPSHOT),
            "--glob",
            fixture_glob("offline_aligned.md"),
        ]
    )

    assert code == 0
    assert stderr == ""
    assert "issue-drift: OK" in stdout
    assert "verdict: PASS" in stdout
    assert "drift threshold: 0" in stdout
    assert "overlap count: 0" in stdout
    assert "max overlap count: 0" in stdout
    assert "max verdict: PASS" in stdout
    assert "checked row count: 1" in stdout


def test_offline_snapshot_with_thresholded_mismatch_reports_drift() -> None:
    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(ISSUES_SNAPSHOT),
            "--glob",
            fixture_glob("offline_mismatch.md"),
            "--drift-threshold",
            "1",
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "issue-drift: DRIFT" in stderr
    assert "found 1 mismatch(es);" in stderr
    assert "drift threshold: 1;" in stderr
    assert "threshold gate: blocked;" in stderr


def test_drift_is_allowed_when_max_verdict_is_drift() -> None:
    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(ISSUES_SNAPSHOT),
            "--glob",
            fixture_glob("offline_mismatch.md"),
            "--drift-threshold",
            "1",
            "--max-verdict",
            "drift",
        ]
    )

    assert code == 0
    assert stderr == ""
    assert "issue-drift: DRIFT" in stdout
    assert "drift threshold: 1;" in stdout
    assert "max verdict: DRIFT;" in stdout
    assert "threshold gate: allowed;" in stdout


def test_negative_drift_threshold_exits_2() -> None:
    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(ISSUES_SNAPSHOT),
            "--glob",
            fixture_glob("offline_aligned.md"),
            "--drift-threshold",
            "-1",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "--drift-threshold must be >= 0" in stderr


def test_negative_max_overlap_count_exits_2() -> None:
    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(ISSUES_SNAPSHOT),
            "--glob",
            fixture_glob("offline_aligned.md"),
            "--max-overlap-count",
            "-1",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "--max-overlap-count must be >= 0" in stderr


def test_overlap_conflict_forces_fail_when_over_threshold(tmp_path: Path) -> None:
    issues_snapshot = tmp_path / "issues_overlap.json"
    issues_snapshot.write_text(
        json.dumps({"open": [101], "closed": [101]}, indent=2) + "\n",
        encoding="utf-8",
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_snapshot),
            "--glob",
            fixture_glob("offline_mismatch.md"),
            "--drift-threshold",
            "1",
            "--max-verdict",
            "drift",
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "issue-drift: FAIL" in stderr
    assert "overlap count: 1;" in stderr
    assert "max overlap count: 0;" in stderr
    assert "threshold gate: blocked;" in stderr


def test_overlap_conflict_can_be_bounded_with_threshold(tmp_path: Path) -> None:
    issues_snapshot = tmp_path / "issues_overlap_allowed.json"
    issues_snapshot.write_text(
        json.dumps({"open": [101], "closed": [101]}, indent=2) + "\n",
        encoding="utf-8",
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_snapshot),
            "--glob",
            fixture_glob("offline_mismatch.md"),
            "--drift-threshold",
            "1",
            "--max-overlap-count",
            "1",
            "--max-verdict",
            "drift",
        ]
    )

    assert code == 0
    assert stderr == ""
    assert "issue-drift: DRIFT" in stdout
    assert "overlap count: 1;" in stdout
    assert "max overlap count: 1;" in stdout
    assert "threshold gate: allowed;" in stdout


def test_classify_verdict_pass_drift_fail() -> None:
    assert (
        check_issue_checkbox_drift.classify_verdict(
            mismatch_count=0,
            drift_threshold=0,
            overlap_count=0,
            max_overlap_count=0,
        )
        == "PASS"
    )
    assert (
        check_issue_checkbox_drift.classify_verdict(
            mismatch_count=1,
            drift_threshold=1,
            overlap_count=0,
            max_overlap_count=0,
        )
        == "DRIFT"
    )
    assert (
        check_issue_checkbox_drift.classify_verdict(
            mismatch_count=2,
            drift_threshold=1,
            overlap_count=0,
            max_overlap_count=0,
        )
        == "FAIL"
    )
    assert (
        check_issue_checkbox_drift.classify_verdict(
            mismatch_count=1,
            drift_threshold=1,
            overlap_count=1,
            max_overlap_count=0,
        )
        == "FAIL"
    )
