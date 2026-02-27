import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path

import pytest

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "extract_open_blockers.py"
REPO_ROOT = Path(__file__).resolve().parents[2]
SPEC = importlib.util.spec_from_file_location("extract_open_blockers", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/extract_open_blockers.py")
extract_open_blockers = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = extract_open_blockers
SPEC.loader.exec_module(extract_open_blockers)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "open_blockers"
VALID_ROOT = FIXTURE_ROOT / "valid"
ZERO_OPEN_ROOT = FIXTURE_ROOT / "zero_open"
MALFORMED_ROOT = FIXTURE_ROOT / "malformed"
EXTRACT_FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "extract_open_blockers"
STRICT_DECODE_FAILURE_ROOT = EXTRACT_FIXTURE_ROOT / "strict_decode_failure"
REPO_ROOT_SCAN_ROOT = EXTRACT_FIXTURE_ROOT / "repo_root_scan"
EXPECTED_REPO_ROOT_EXCLUSION_JSON = EXTRACT_FIXTURE_ROOT / "expected_repo_root_exclusion.json"
SNAPSHOT_GENERATED_AT_UTC = "2026-02-25T01:02:03Z"
SNAPSHOT_SOURCE = "fixture:tests/tooling/fixtures/open_blockers/valid"


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = extract_open_blockers.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def repo_relative(path: Path) -> str:
    return path.resolve().relative_to(REPO_ROOT).as_posix()


def test_json_output_matches_expected_fixture() -> None:
    expected_text = (FIXTURE_ROOT / "expected_valid.json").read_text(encoding="utf-8")
    expected_payload = json.loads(expected_text)

    code, output, stderr = run_main(["--root", str(VALID_ROOT), "--format", "json"])

    assert code == 0
    assert stderr == ""
    assert output == expected_text

    payload = json.loads(output)
    assert payload == expected_payload
    assert payload[0]["blocker_id"] == "BLK-900-01"
    assert payload[1]["blocker_id"] == "BLK-900-02"
    assert payload[2]["due_date_utc"] is None


def test_markdown_output_matches_expected_fixture() -> None:
    expected = (FIXTURE_ROOT / "expected_valid.md").read_text(encoding="utf-8")

    code, output, stderr = run_main(["--root", str(VALID_ROOT), "--format", "markdown"])

    assert code == 0
    assert stderr == ""
    assert output == expected
    assert "| blocker_id | source_path | line | owner | due_date_utc | summary | status |" in output


def test_zero_open_json_returns_success() -> None:
    code, output, stderr = run_main(["--root", str(ZERO_OPEN_ROOT), "--format", "json"])

    assert code == 0
    assert stderr == ""
    assert output == "[]\n"


def test_snapshot_json_output_matches_expected_fixture() -> None:
    expected_text = (FIXTURE_ROOT / "expected_valid.snapshot.json").read_text(encoding="utf-8")
    expected_payload = json.loads(expected_text)

    code, output, stderr = run_main(
        [
            "--root",
            str(VALID_ROOT),
            "--format",
            "snapshot-json",
            "--generated-at-utc",
            SNAPSHOT_GENERATED_AT_UTC,
            "--source",
            SNAPSHOT_SOURCE,
        ]
    )

    assert code == 0
    assert stderr == ""
    assert output == expected_text

    payload = json.loads(output)
    assert payload == expected_payload
    assert list(payload.keys()) == [
        "generated_at_utc",
        "source",
        "open_blocker_count",
        "open_blockers",
    ]
    assert payload["generated_at_utc"] == SNAPSHOT_GENERATED_AT_UTC
    assert payload["source"] == SNAPSHOT_SOURCE
    assert payload["open_blocker_count"] == 3

    rows = payload["open_blockers"]
    assert len(rows) == 3
    assert [
        (row["source_path"], row["line_number"], row["blocker_id"])
        for row in rows
    ] == [
        ("tests/tooling/fixtures/open_blockers/valid/issue_alpha.md", 5, "BLK-900-01"),
        ("tests/tooling/fixtures/open_blockers/valid/issue_alpha.md", 6, "BLK-900-02"),
        ("tests/tooling/fixtures/open_blockers/valid/issue_beta.md", 7, "BLK-901-01"),
    ]
    assert [
        (row["source_path"], row["line_number"], row["blocker_id"])
        for row in rows
    ] == sorted(
        [
            (row["source_path"], row["line_number"], row["blocker_id"])
            for row in rows
        ],
        key=lambda item: (item[0].casefold(), item[0], item[1], item[2].casefold(), item[2]),
    )
    for row in rows:
        assert list(row.keys()) == ["blocker_id", "source_path", "line_number", "line"]
        assert row["line_number"] == row["line"]


def test_zero_open_snapshot_json_returns_success() -> None:
    code, output, stderr = run_main(
        [
            "--root",
            str(ZERO_OPEN_ROOT),
            "--format",
            "snapshot-json",
            "--generated-at-utc",
            SNAPSHOT_GENERATED_AT_UTC,
            "--source",
            "fixture:tests/tooling/fixtures/open_blockers/zero_open",
        ]
    )

    assert code == 0
    assert stderr == ""

    payload = json.loads(output)
    assert list(payload.keys()) == [
        "generated_at_utc",
        "source",
        "open_blocker_count",
        "open_blockers",
    ]
    assert payload["generated_at_utc"] == SNAPSHOT_GENERATED_AT_UTC
    assert payload["source"] == "fixture:tests/tooling/fixtures/open_blockers/zero_open"
    assert payload["open_blocker_count"] == 0
    assert payload["open_blockers"] == []


@pytest.mark.parametrize(
    ("extra_args", "expected_error"),
    (
        (
            [],
            "--generated-at-utc is required when --format snapshot-json",
        ),
        (
            ["--source", SNAPSHOT_SOURCE],
            "--generated-at-utc is required when --format snapshot-json",
        ),
        (
            ["--generated-at-utc", SNAPSHOT_GENERATED_AT_UTC],
            "--source is required when --format snapshot-json",
        ),
    ),
)
def test_snapshot_json_requires_generated_at_utc_and_source(
    extra_args: list[str],
    expected_error: str,
) -> None:
    code, output, stderr = run_main(
        ["--root", str(VALID_ROOT), "--format", "snapshot-json", *extra_args]
    )

    assert code == 2
    assert output == ""
    assert expected_error in stderr


@pytest.mark.parametrize(
    ("generated_at_utc", "expected_error"),
    (
        (
            "2026-02-25T01:02:03",
            "invalid --generated-at-utc: expected strict UTC timestamp",
        ),
        (
            " 2026-02-25T01:02:03Z ",
            "invalid --generated-at-utc: value must not include leading or trailing whitespace",
        ),
        (
            "2026-02-30T01:02:03Z",
            "invalid --generated-at-utc: timestamp is not a valid UTC date-time",
        ),
    ),
)
def test_snapshot_json_rejects_invalid_generated_at_utc(
    generated_at_utc: str,
    expected_error: str,
) -> None:
    code, output, stderr = run_main(
        [
            "--root",
            str(VALID_ROOT),
            "--format",
            "snapshot-json",
            "--generated-at-utc",
            generated_at_utc,
            "--source",
            SNAPSHOT_SOURCE,
        ]
    )

    assert code == 2
    assert output == ""
    assert expected_error in stderr


@pytest.mark.parametrize(
    ("source", "expected_error"),
    (
        (
            "",
            "invalid --source: value must be a non-empty canonical string",
        ),
        (
            " fixture:tests/tooling/fixtures/open_blockers/valid ",
            "invalid --source: value must not include leading or trailing whitespace",
        ),
        (
            "fixture:tests/tooling/fixtures/open_blockers/valid  manual",
            "invalid --source: value must be canonical (no repeated internal whitespace)",
        ),
    ),
)
def test_snapshot_json_rejects_invalid_source(
    source: str,
    expected_error: str,
) -> None:
    code, output, stderr = run_main(
        [
            "--root",
            str(VALID_ROOT),
            "--format",
            "snapshot-json",
            "--generated-at-utc",
            SNAPSHOT_GENERATED_AT_UTC,
            "--source",
            source,
        ]
    )

    assert code == 2
    assert output == ""
    assert expected_error in stderr


def test_malformed_open_blocker_row_fails_with_actionable_error() -> None:
    code, output, stderr = run_main(["--root", str(MALFORMED_ROOT), "--format", "json"])

    assert code == 2
    assert output == ""
    assert "missing_owner.md:5" in stderr
    assert "malformed OPEN blocker row" in stderr
    assert "owner value is empty" in stderr


def test_strict_decode_failure_reports_explicit_path_diagnostics() -> None:
    code, output, stderr = run_main(
        ["--root", str(STRICT_DECODE_FAILURE_ROOT), "--format", "json"]
    )

    assert code == 2
    assert output == ""
    assert stderr == (
        "error: markdown file is not valid UTF-8: "
        "tests/tooling/fixtures/extract_open_blockers/strict_decode_failure/invalid_utf8.md "
        "(decode failure at byte 10)\n"
    )


def test_exclusion_mode_repo_root_scan_succeeds_with_deterministic_order() -> None:
    expected_text = EXPECTED_REPO_ROOT_EXCLUSION_JSON.read_text(encoding="utf-8")
    excluded_file_path = repo_relative(
        REPO_ROOT_SCAN_ROOT / "third_party" / "invalid_utf8.md"
    )
    excluded_glob = "tests/tooling/fixtures/extract_open_blockers/repo_root_scan/third_party/**/*.md"
    args_a = [
        "--root",
        str(REPO_ROOT_SCAN_ROOT),
        "--format",
        "json",
        "--exclude-path",
        excluded_file_path,
        "--exclude-path",
        excluded_glob,
        "--exclude-path",
        excluded_file_path,
    ]
    args_b = [
        "--root",
        str(REPO_ROOT_SCAN_ROOT),
        "--format",
        "json",
        "--exclude-path",
        excluded_glob,
        "--exclude-path",
        excluded_file_path,
    ]

    first_code, first_output, first_stderr = run_main(args_a)
    second_code, second_output, second_stderr = run_main(args_b)

    assert first_code == 0
    assert second_code == 0
    assert first_stderr == second_stderr == ""
    assert first_output == second_output == expected_text

    payload = json.loads(first_output)
    ordering = [
        (row["source_path"], row["line"], row["blocker_id"])
        for row in payload
    ]
    assert ordering == sorted(
        ordering,
        key=lambda item: (item[0].casefold(), item[0], item[1], item[2].casefold(), item[2]),
    )
    assert all("invalid_utf8.md" not in row["source_path"] for row in payload)
