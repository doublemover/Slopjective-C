import importlib.util
import io
import json
import os
import subprocess
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "extract_remaining_tasks.py"
SPEC = importlib.util.spec_from_file_location("extract_remaining_tasks", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/extract_remaining_tasks.py")
extract_remaining_tasks = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = extract_remaining_tasks
SPEC.loader.exec_module(extract_remaining_tasks)

FIXTURE_DIR = Path(__file__).resolve().parent / "fixtures" / "remaining_tasks"
STATUS_INTEGRITY_FIXTURE_DIR = (
    Path(__file__).resolve().parent / "fixtures" / "remaining_tasks_status_integrity"
)
VALID_CATALOG_PATH = STATUS_INTEGRITY_FIXTURE_DIR / "catalog_valid_status.json"
MISSING_STATUS_CATALOG_PATH = STATUS_INTEGRITY_FIXTURE_DIR / "catalog_missing_status.json"
EXPECTED_INPUT_PATH = "tests/tooling/fixtures/remaining_tasks/catalog_fixture.json"
VALID_INPUT_PATH = (
    "tests/tooling/fixtures/remaining_tasks_status_integrity/catalog_valid_status.json"
)


def run_main(argv: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = extract_remaining_tasks.main(argv)
    return code, stdout.getvalue(), stderr.getvalue()


def align_expected_input_path(expected: str) -> str:
    return expected.replace(EXPECTED_INPUT_PATH, VALID_INPUT_PATH)


def write_catalog(path: Path, *, tasks: list[dict[str, object]]) -> None:
    payload = {
        "generated_on": "2026-02-24",
        "task_count": len(tasks),
        "tasks": tasks,
    }
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def test_default_json_output_matches_expected_fixture() -> None:
    expected = align_expected_input_path(
        (FIXTURE_DIR / "expected_default_lane.json").read_text(encoding="utf-8")
    )
    code, output, stderr = run_main(
        [
            "--input",
            str(VALID_CATALOG_PATH),
            "--format",
            "json",
            "--group-by",
            "lane",
        ]
    )

    assert code == 0
    assert stderr == ""
    assert output == expected

    payload = json.loads(output)
    assert payload["summary"]["total_tasks"] == 6
    assert [entry["status"] for entry in payload["summary"]["status_counts"]] == [
        "open",
        "open-blocked",
        "blocked",
    ]
    assert payload["summary"]["dispatch_intake"]["status"] == "pass"
    assert payload["summary"]["dispatch_intake"]["recommendation"] == "go"
    assert payload["summary"]["overlap_conflicts"]["count"] == 0
    assert payload["summary"]["overlap_conflicts"]["max_allowed"] == 0
    assert payload["summary"]["overlap_conflicts"]["status"] == "pass"
    assert payload["summary"]["global_capacity"]["active_issue_count"] == 6
    assert payload["summary"]["global_capacity"]["global_wip_cap"] == 16
    assert payload["summary"]["global_capacity"]["status"] == "pass"
    assert payload["overlap_conflicts"] == []
    assert [entry["status"] for entry in payload["summary"]["capacity_status_counts"]] == ["pass"]
    assert [entry["lane"] for entry in payload["capacity"]] == ["A", "B", "C", "D"]
    assert [task["task_id"] for task in payload["tasks"]] == [
        "SPT-2003",
        "SPT-2002",
        "SPT-2007",
        "SPT-2005",
        "SPT-2008",
        "SPT-2001",
    ]


def test_markdown_output_matches_expected_fixture() -> None:
    expected = align_expected_input_path(
        (FIXTURE_DIR / "expected_default_lane.md").read_text(encoding="utf-8")
    )
    code, output, stderr = run_main(
        [
            "--input",
            str(VALID_CATALOG_PATH),
            "--format",
            "markdown",
            "--group-by",
            "lane",
        ]
    )

    assert code == 0
    assert stderr == ""
    assert output == expected
    assert "| task_id | title | lane | status | path | line |" in output


def test_repeatable_filters_are_deduped_and_sorted_deterministically() -> None:
    code, output, stderr = run_main(
        [
            "--input",
            str(VALID_CATALOG_PATH),
            "--format",
            "json",
            "--group-by",
            "status",
            "--status",
            "blocked",
            "--status",
            "open",
            "--status",
            "open",
            "--lane",
            "b",
            "--lane",
            "A",
            "--lane",
            "B",
        ]
    )

    assert code == 0
    assert stderr == ""

    payload = json.loads(output)
    assert payload["filters"]["status"] == ["open", "blocked"]
    assert payload["filters"]["lane"] == ["A", "B"]
    assert payload["summary"]["total_tasks"] == 5
    assert [group["group"] for group in payload["groups"]] == ["open", "blocked"]
    assert [task["task_id"] for task in payload["tasks"]] == [
        "SPT-2005",
        "SPT-2007",
        "SPT-2001",
        "SPT-2002",
        "SPT-2008",
    ]


def test_missing_status_fails_without_override_flag() -> None:
    code, output, stderr = run_main(
        [
            "--input",
            str(MISSING_STATUS_CATALOG_PATH),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert output == ""
    assert "missing required 'execution_status'" in stderr
    assert "--allow-missing-status" in stderr
    assert "SPT-2006" in stderr


def test_allow_missing_status_override_includes_missing_group() -> None:
    code, output, stderr = run_main(
        [
            "--input",
            str(MISSING_STATUS_CATALOG_PATH),
            "--format",
            "json",
            "--group-by",
            "status",
            "--status",
            "missing",
            "--allow-missing-status",
        ]
    )

    assert code == 0
    assert stderr == ""

    payload = json.loads(output)
    assert payload["summary"]["total_tasks"] == 1
    assert payload["groups"][0]["group"] == "missing"
    assert payload["groups"][0]["tasks"][0]["task_id"] == "SPT-2006"


def test_subprocess_output_is_stable_across_hash_seeds() -> None:
    cmd = [
        sys.executable,
        str(SCRIPT_PATH),
        "--input",
        str(VALID_CATALOG_PATH),
        "--format",
        "markdown",
        "--group-by",
        "path",
    ]

    def run_with_hash_seed(seed: str) -> tuple[int, bytes, bytes]:
        env = os.environ.copy()
        env["PYTHONHASHSEED"] = seed
        result = subprocess.run(cmd, capture_output=True, check=False, env=env)
        return result.returncode, result.stdout, result.stderr

    rc1, stdout1, stderr1 = run_with_hash_seed("1")
    rc2, stdout2, stderr2 = run_with_hash_seed("2")

    assert rc1 == 0, stderr1.decode("utf-8", errors="replace")
    assert rc2 == 0, stderr2.decode("utf-8", errors="replace")
    assert stderr1 == b""
    assert stderr2 == b""
    assert stdout1 == stdout2
    assert b"\r" not in stdout1


def test_classify_capacity_status_threshold_edges() -> None:
    assert extract_remaining_tasks.classify_capacity_status(0.85) == "pass"
    assert extract_remaining_tasks.classify_capacity_status(1.00) == "drift"
    assert extract_remaining_tasks.classify_capacity_status(1.0001) == "fail"


def test_dispatch_intake_enforcement_reports_drift_when_threshold_is_pass(tmp_path: Path) -> None:
    catalog_path = tmp_path / "catalog_drift.json"
    write_catalog(
        catalog_path,
        tasks=[
            {
                "task_id": f"SPT-91{index:02d}",
                "title": f"Lane A active task {index}",
                "path": f"spec/planning/a{index}.md",
                "line": index + 1,
                "lane": "A",
                "execution_status": "open",
            }
            for index in range(4)
        ],
    )

    code, output, stderr = run_main(
        [
            "--input",
            str(catalog_path),
            "--format",
            "json",
            "--max-dispatch-intake-status",
            "pass",
        ]
    )

    assert code == 1
    payload = json.loads(output)
    assert payload["summary"]["dispatch_intake"]["status"] == "drift"
    assert payload["summary"]["dispatch_intake"]["recommendation"] == "hold"
    assert "status=drift" in stderr
    assert "max_allowed=pass" in stderr


def test_dispatch_intake_enforcement_reports_fail_when_lane_exceeds_cap(tmp_path: Path) -> None:
    catalog_path = tmp_path / "catalog_fail.json"
    write_catalog(
        catalog_path,
        tasks=[
            {
                "task_id": f"SPT-92{index:02d}",
                "title": f"Lane D active task {index}",
                "path": f"spec/planning/d{index}.md",
                "line": index + 1,
                "lane": "D",
                "execution_status": "open",
            }
            for index in range(4)
        ],
    )

    code, output, stderr = run_main(
        [
            "--input",
            str(catalog_path),
            "--format",
            "json",
            "--max-dispatch-intake-status",
            "drift",
        ]
    )

    assert code == 1
    payload = json.loads(output)
    assert payload["summary"]["dispatch_intake"]["status"] == "fail"
    assert payload["summary"]["dispatch_intake"]["recommendation"] == "no-go"
    assert "status=fail" in stderr
    assert "max_allowed=drift" in stderr


def test_overlap_conflict_default_threshold_forces_fail(tmp_path: Path) -> None:
    catalog_path = tmp_path / "catalog_overlap_fail.json"
    write_catalog(
        catalog_path,
        tasks=[
            {
                "task_id": "SPT-9300",
                "title": "Lane A open path overlap",
                "path": "spec/planning/shared.md",
                "line": 10,
                "lane": "A",
                "execution_status": "open",
            },
            {
                "task_id": "SPT-9301",
                "title": "Lane B open path overlap",
                "path": "spec/planning/shared.md",
                "line": 11,
                "lane": "B",
                "execution_status": "open",
            },
        ],
    )

    code, output, stderr = run_main(
        [
            "--input",
            str(catalog_path),
            "--format",
            "json",
            "--max-dispatch-intake-status",
            "drift",
        ]
    )

    assert code == 1
    payload = json.loads(output)
    assert payload["summary"]["overlap_conflicts"]["count"] == 1
    assert payload["summary"]["overlap_conflicts"]["status"] == "fail"
    assert payload["summary"]["dispatch_intake"]["status"] == "fail"
    assert payload["overlap_conflicts"][0]["path"] == "spec/planning/shared.md"
    assert payload["overlap_conflicts"][0]["lane_count"] == 2
    assert payload["overlap_conflicts"][0]["lanes"] == ["A", "B"]
    assert "status=fail" in stderr
    assert "max_allowed=drift" in stderr


def test_overlap_conflict_threshold_allows_drift_when_bounded(tmp_path: Path) -> None:
    catalog_path = tmp_path / "catalog_overlap_drift.json"
    write_catalog(
        catalog_path,
        tasks=[
            {
                "task_id": "SPT-9310",
                "title": "Lane A open path overlap",
                "path": "spec/planning/shared.md",
                "line": 10,
                "lane": "A",
                "execution_status": "open",
            },
            {
                "task_id": "SPT-9311",
                "title": "Lane B open path overlap",
                "path": "spec/planning/shared.md",
                "line": 11,
                "lane": "B",
                "execution_status": "open",
            },
        ],
    )

    code, output, stderr = run_main(
        [
            "--input",
            str(catalog_path),
            "--format",
            "json",
            "--max-overlap-conflicts",
            "1",
            "--max-dispatch-intake-status",
            "drift",
        ]
    )

    assert code == 0
    assert stderr == ""
    payload = json.loads(output)
    assert payload["summary"]["overlap_conflicts"]["count"] == 1
    assert payload["summary"]["overlap_conflicts"]["max_allowed"] == 1
    assert payload["summary"]["overlap_conflicts"]["status"] == "drift"
    assert payload["summary"]["dispatch_intake"]["status"] == "drift"
    assert payload["summary"]["dispatch_intake"]["recommendation"] == "hold"


def test_negative_max_overlap_conflicts_exits_2(tmp_path: Path) -> None:
    catalog_path = tmp_path / "catalog_negative_overlap_threshold.json"
    write_catalog(
        catalog_path,
        tasks=[
            {
                "task_id": "SPT-9320",
                "title": "Lane A active task",
                "path": "spec/planning/a.md",
                "line": 1,
                "lane": "A",
                "execution_status": "open",
            }
        ],
    )

    code, output, stderr = run_main(
        [
            "--input",
            str(catalog_path),
            "--max-overlap-conflicts",
            "-1",
        ]
    )

    assert code == 2
    assert output == ""
    assert "--max-overlap-conflicts must be >= 0" in stderr


def test_global_capacity_counts_unknown_lane_rows(tmp_path: Path) -> None:
    catalog_path = tmp_path / "catalog_unknown_lane.json"
    write_catalog(
        catalog_path,
        tasks=[
            {
                "task_id": "SPT-9330",
                "title": "Unknown lane task",
                "path": "spec/planning/unknown.md",
                "line": 1,
                "lane": "E",
                "execution_status": "open",
            },
            {
                "task_id": "SPT-9331",
                "title": "Known lane task",
                "path": "spec/planning/known.md",
                "line": 2,
                "lane": "A",
                "execution_status": "open",
            },
        ],
    )

    code, output, stderr = run_main(
        [
            "--input",
            str(catalog_path),
            "--format",
            "json",
        ]
    )

    assert code == 0
    assert stderr == ""
    payload = json.loads(output)
    assert payload["summary"]["global_capacity"]["active_issue_count"] == 2
