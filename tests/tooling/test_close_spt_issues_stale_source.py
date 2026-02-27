import importlib.util
import sys
from pathlib import Path

SCRIPT_PATH = (
    Path(__file__).resolve().parents[2] / "scripts" / "close_spt_issues_from_checkboxes.py"
)
SPEC = importlib.util.spec_from_file_location("close_spt_issues_from_checkboxes", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/close_spt_issues_from_checkboxes.py for tests.")
close_spt_issues_from_checkboxes = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = close_spt_issues_from_checkboxes
SPEC.loader.exec_module(close_spt_issues_from_checkboxes)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "hb02_close"


def issue_ref(number: int, task_id: str):
    return close_spt_issues_from_checkboxes.IssueRef(
        number=number,
        title=f"[{task_id}][Lane A] Fixture",
        url=f"https://example.test/issues/{number}",
    )


def test_stale_entries_are_excluded_from_candidates(capsys, monkeypatch) -> None:
    catalog = FIXTURE_ROOT / "catalog_stale_mixed.json"
    monkeypatch.setattr(
        close_spt_issues_from_checkboxes,
        "fetch_open_spt_issues",
        lambda _: {
            "SPT-9001": issue_ref(101, "SPT-9001"),
            "SPT-9002": issue_ref(102, "SPT-9002"),
            "SPT-9003": issue_ref(103, "SPT-9003"),
            "SPT-9004": issue_ref(104, "SPT-9004"),
            "SPT-9005": issue_ref(105, "SPT-9005"),
        },
    )

    code = close_spt_issues_from_checkboxes.main(["--catalog", str(catalog)])

    captured = capsys.readouterr()
    assert code == 0
    assert captured.err == ""
    assert "checked_open_matches=1" in captured.out
    assert "stale_source=3" in captured.out
    assert "match SPT-9001 -> #101" in captured.out
    assert "SPT-9002" not in captured.out
    assert "SPT-9003" not in captured.out
    assert "SPT-9004" not in captured.out
    assert "SPT-9005" not in captured.out


def test_fail_on_stale_source_exits_2_with_diagnostics(capsys, monkeypatch) -> None:
    catalog = FIXTURE_ROOT / "catalog_stale_mixed.json"

    def fail_if_gh_called(_):
        raise AssertionError("fetch_open_spt_issues should not be called when stale source fails fast")

    monkeypatch.setattr(
        close_spt_issues_from_checkboxes,
        "fetch_open_spt_issues",
        fail_if_gh_called,
    )

    code = close_spt_issues_from_checkboxes.main(
        ["--catalog", str(catalog), "--fail-on-stale-source"]
    )

    captured = capsys.readouterr()
    assert code == 2
    assert "stale_source SPT-9002" in captured.err
    assert "reason=hash-mismatch" in captured.err
    assert "stale_source SPT-9003" in captured.err
    assert "reason=missing-file" in captured.err
    assert "stale_source SPT-9004" in captured.err
    assert "reason=line-out-of-range" in captured.err
    assert "detected 3 stale source entries" in captured.err

    line_9002 = captured.err.find("stale_source SPT-9002")
    line_9003 = captured.err.find("stale_source SPT-9003")
    line_9004 = captured.err.find("stale_source SPT-9004")
    assert line_9002 != -1
    assert line_9003 != -1
    assert line_9004 != -1
    assert line_9002 < line_9003 < line_9004


def test_matching_hash_allows_apply_close_for_checked_rows(tmp_path, capsys, monkeypatch) -> None:
    catalog = FIXTURE_ROOT / "catalog_valid_hash_only.json"
    monkeypatch.setattr(
        close_spt_issues_from_checkboxes,
        "fetch_open_spt_issues",
        lambda _: {
            "SPT-9101": issue_ref(201, "SPT-9101"),
            "SPT-9102": issue_ref(202, "SPT-9102"),
        },
    )

    closed_calls: list[tuple[int, str]] = []

    def record_close(number: int, comment: str) -> None:
        closed_calls.append((number, comment))

    monkeypatch.setattr(close_spt_issues_from_checkboxes, "close_issue", record_close)

    plan_path = tmp_path / "hb02-plan.json"
    plan_code = close_spt_issues_from_checkboxes.main(
        [
            "--catalog",
            str(catalog),
            "--plan-out",
            str(plan_path),
            "--commit-sha",
            "deadbeef",
        ]
    )
    assert plan_code == 0

    code = close_spt_issues_from_checkboxes.main(
        [
            "--catalog",
            str(catalog),
            "--apply",
            "--plan-in",
            str(plan_path),
            "--commit-sha",
            "deadbeef",
        ]
    )

    captured = capsys.readouterr()
    assert code == 0
    assert len(closed_calls) == 1
    assert closed_calls[0][0] == 201
    assert "- Task ID: `SPT-9101`" in closed_calls[0][1]
    assert "- Commit evidence: `deadbeef`" in closed_calls[0][1]
    assert "match SPT-9101 -> #201" in captured.out
    assert "SPT-9102" not in captured.out
    assert "closed_total=1" in captured.out
