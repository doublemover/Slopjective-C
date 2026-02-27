import importlib.util
import json
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

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "hb04_plan_apply"


def issue_ref(number: int, task_id: str):
    return close_spt_issues_from_checkboxes.IssueRef(
        number=number,
        title=f"[{task_id}][Lane A] Fixture",
        url=f"https://example.test/issues/{number}",
    )


def plan_open_issue_map() -> dict[str, object]:
    return {
        "SPT-9201": issue_ref(301, "SPT-9201"),
        "SPT-9202": issue_ref(302, "SPT-9202"),
        "SPT-9203": issue_ref(303, "SPT-9203"),
    }


def test_apply_without_plan_in_exits_2(capsys, monkeypatch) -> None:
    catalog = FIXTURE_ROOT / "catalog.json"

    def fail_if_gh_called(_):
        raise AssertionError("fetch_open_spt_issues should not be called when --plan-in is missing")

    monkeypatch.setattr(
        close_spt_issues_from_checkboxes,
        "fetch_open_spt_issues",
        fail_if_gh_called,
    )

    code = close_spt_issues_from_checkboxes.main(["--catalog", str(catalog), "--apply"])

    captured = capsys.readouterr()
    assert code == 2
    assert "--apply requires --plan-in" in captured.err


def test_edited_plan_exits_2_and_closes_zero_issues(tmp_path, capsys, monkeypatch) -> None:
    catalog = FIXTURE_ROOT / "catalog.json"
    plan_path = tmp_path / "close-plan.json"
    monkeypatch.setattr(
        close_spt_issues_from_checkboxes,
        "fetch_open_spt_issues",
        lambda _: plan_open_issue_map(),
    )

    plan_code = close_spt_issues_from_checkboxes.main(
        [
            "--catalog",
            str(catalog),
            "--lane",
            "A",
            "--plan-out",
            str(plan_path),
            "--commit-sha",
            "abc123",
        ]
    )
    assert plan_code == 0

    plan_payload = json.loads(plan_path.read_text(encoding="utf-8"))
    plan_payload["candidate_issue_ids"][0] = 999
    plan_path.write_text(json.dumps(plan_payload, indent=2) + "\n", encoding="utf-8")

    closed_calls: list[tuple[int, str]] = []
    monkeypatch.setattr(
        close_spt_issues_from_checkboxes,
        "close_issue",
        lambda number, comment: closed_calls.append((number, comment)),
    )

    def fail_if_gh_called(_):
        raise AssertionError("fetch_open_spt_issues should not be called during --apply --plan-in")

    monkeypatch.setattr(
        close_spt_issues_from_checkboxes,
        "fetch_open_spt_issues",
        fail_if_gh_called,
    )

    apply_code = close_spt_issues_from_checkboxes.main(
        ["--catalog", str(catalog), "--apply", "--plan-in", str(plan_path)]
    )

    captured = capsys.readouterr()
    assert apply_code == 2
    assert "plan digest mismatch" in captured.err
    assert closed_calls == []


def test_valid_plan_in_apply_closes_expected_issue_set(tmp_path, capsys, monkeypatch) -> None:
    catalog = FIXTURE_ROOT / "catalog.json"
    plan_path = tmp_path / "close-plan.json"
    monkeypatch.setattr(
        close_spt_issues_from_checkboxes,
        "fetch_open_spt_issues",
        lambda _: plan_open_issue_map(),
    )

    plan_code = close_spt_issues_from_checkboxes.main(
        [
            "--catalog",
            str(catalog),
            "--lane",
            "A",
            "--plan-out",
            str(plan_path),
            "--commit-sha",
            "deadbeef",
        ]
    )

    plan_payload = json.loads(plan_path.read_text(encoding="utf-8"))
    assert plan_code == 0
    assert "catalog_digest" in plan_payload
    assert "generated_at" in plan_payload
    assert plan_payload["lane_filter"] == "A"
    assert plan_payload["task_id_prefix"] == "SPT-"
    assert plan_payload["commit_sha"] == "deadbeef"
    assert plan_payload["candidate_task_ids"] == ["SPT-9201", "SPT-9202"]
    assert plan_payload["candidate_issue_ids"] == [301, 302]

    closed_calls: list[tuple[int, str]] = []

    def record_close(number: int, comment: str) -> None:
        closed_calls.append((number, comment))

    monkeypatch.setattr(close_spt_issues_from_checkboxes, "close_issue", record_close)

    def fail_if_gh_called(_):
        raise AssertionError("fetch_open_spt_issues should not be called during --apply --plan-in")

    monkeypatch.setattr(
        close_spt_issues_from_checkboxes,
        "fetch_open_spt_issues",
        fail_if_gh_called,
    )

    apply_code = close_spt_issues_from_checkboxes.main(
        ["--catalog", str(catalog), "--apply", "--plan-in", str(plan_path)]
    )

    captured = capsys.readouterr()
    assert apply_code == 0
    assert [number for number, _ in closed_calls] == [301, 302]
    assert all("- Commit evidence: `deadbeef`" in comment for _, comment in closed_calls)
    assert "SPT-9203" not in captured.out
    assert "closed_total=2" in captured.out
