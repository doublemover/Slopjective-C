from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path
from typing import Any

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "capture_activation_snapshots.py"
SPEC = importlib.util.spec_from_file_location("capture_activation_snapshots", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/capture_activation_snapshots.py for tests.")
capture_activation_snapshots = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = capture_activation_snapshots
SPEC.loader.exec_module(capture_activation_snapshots)


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def install_fake_client(
    monkeypatch: Any,
    *,
    issues: list[dict[str, Any]],
    milestone_pages: list[list[dict[str, Any]]],
) -> None:
    class FakeGhClient:
        def __init__(self, *, root: Path) -> None:
            self.root = root

        def list_issues(self, *, state: str) -> list[dict[str, Any]]:
            assert state == "open"
            return issues

        def api_json(self, endpoint: str, *, paginate: bool = False) -> Any:
            assert endpoint == capture_activation_snapshots.MILESTONES_ENDPOINT
            assert paginate is True
            return milestone_pages

    monkeypatch.setattr(capture_activation_snapshots, "GhClient", FakeGhClient)


def test_main_writes_sorted_contract_snapshots(monkeypatch: Any, tmp_path: Path, capsys: Any) -> None:
    install_fake_client(
        monkeypatch,
        issues=[
            {
                "number": 10,
                "title": "Issue ten",
                "state": "open",
                "url": "https://example.test/issues/10",
                "html_url": "https://example.test/html/10",
                "closed_at": None,
                "labels": [{"name": "zeta"}, {"name": "alpha"}, {"name": "alpha"}],
                "milestone": {"number": 4, "title": "Milestone four"},
            },
            {
                "number": 3,
                "title": "Issue three",
                "state": "open",
                "url": "https://example.test/issues/3",
                "labels": ["beta", {"name": "alpha"}],
            },
            {
                "number": 8,
                "title": "Issue eight",
                "state": "open",
                "labels": [],
            },
        ],
        milestone_pages=[
            [
                {
                    "number": 35,
                    "title": "Activation Snapshot Integrity W1",
                    "state": "open",
                    "description": "Current batch milestone",
                    "open_issues": 5,
                    "closed_issues": 0,
                    "url": "https://example.test/api/milestones/35",
                    "html_url": "https://example.test/milestones/35",
                }
            ],
            [
                {
                    "number": 7,
                    "title": "Earlier milestone",
                    "state": "open",
                    "open_issues": 0,
                    "closed_issues": 12,
                    "url": "https://example.test/api/milestones/7",
                    "html_url": "https://example.test/milestones/7",
                }
            ],
        ],
    )

    issues_output = tmp_path / "open_issues_snapshot.json"
    milestones_output = tmp_path / "open_milestones_snapshot.json"
    generated_at_utc = "2026-02-23T20:00:00Z"

    code = capture_activation_snapshots.main(
        [
            "--issues-output",
            str(issues_output),
            "--milestones-output",
            str(milestones_output),
            "--generated-at-utc",
            generated_at_utc,
        ]
    )

    assert code == 0
    captured = capsys.readouterr()
    assert captured.err == ""
    assert "capture-activation-snapshots: OK" in captured.out

    issues_payload = read_json(issues_output)
    assert list(issues_payload.keys()) == [
        "generated_at_utc",
        "source",
        "count",
        "items",
    ]
    assert issues_payload["generated_at_utc"] == generated_at_utc
    assert issues_payload["source"] == capture_activation_snapshots.ISSUES_SOURCE
    assert issues_payload["count"] == 3
    issues_items = issues_payload["items"]
    assert isinstance(issues_items, list)
    assert [item["number"] for item in issues_items] == [3, 8, 10]
    assert issues_items[0]["labels"] == ["alpha", "beta"]
    assert issues_items[1]["milestone"] is None
    assert issues_items[2]["milestone"] == {"number": 4, "title": "Milestone four"}

    milestones_payload = read_json(milestones_output)
    assert list(milestones_payload.keys()) == [
        "generated_at_utc",
        "source",
        "count",
        "items",
    ]
    assert milestones_payload["generated_at_utc"] == generated_at_utc
    assert milestones_payload["source"] == capture_activation_snapshots.MILESTONES_SOURCE
    assert milestones_payload["count"] == 2
    milestone_items = milestones_payload["items"]
    assert isinstance(milestone_items, list)
    assert [item["number"] for item in milestone_items] == [7, 35]

    assert b"\r" not in issues_output.read_bytes()
    assert b"\r" not in milestones_output.read_bytes()


def test_main_uses_source_date_epoch_when_generated_at_is_omitted(
    monkeypatch: Any, tmp_path: Path
) -> None:
    install_fake_client(monkeypatch, issues=[], milestone_pages=[[]])
    monkeypatch.setenv("SOURCE_DATE_EPOCH", "1771804800")

    issues_output = tmp_path / "issues.json"
    milestones_output = tmp_path / "milestones.json"

    code = capture_activation_snapshots.main(
        [
            "--issues-output",
            str(issues_output),
            "--milestones-output",
            str(milestones_output),
        ]
    )

    assert code == 0

    expected_generated_at = "2026-02-23T00:00:00Z"
    issues_payload = read_json(issues_output)
    milestones_payload = read_json(milestones_output)
    assert issues_payload["generated_at_utc"] == expected_generated_at
    assert milestones_payload["generated_at_utc"] == expected_generated_at
    assert issues_payload["count"] == 0
    assert milestones_payload["count"] == 0


def test_main_rejects_invalid_generated_at_utc(monkeypatch: Any, tmp_path: Path, capsys: Any) -> None:
    install_fake_client(monkeypatch, issues=[], milestone_pages=[[]])

    issues_output = tmp_path / "issues.json"
    milestones_output = tmp_path / "milestones.json"
    code = capture_activation_snapshots.main(
        [
            "--issues-output",
            str(issues_output),
            "--milestones-output",
            str(milestones_output),
            "--generated-at-utc",
            "2026-02-23",
        ]
    )

    assert code == 1
    captured = capsys.readouterr()
    assert captured.out == ""
    assert "capture-activation-snapshots:" in captured.err
    assert "--generated-at-utc must be RFC3339 UTC" in captured.err
    assert not issues_output.exists()
    assert not milestones_output.exists()


def test_main_is_byte_stable_for_identical_inputs(monkeypatch: Any, tmp_path: Path) -> None:
    install_fake_client(
        monkeypatch,
        issues=[
            {
                "number": 2,
                "title": "Issue two",
                "state": "open",
                "labels": [{"name": "lane:a"}],
            },
            {
                "number": 1,
                "title": "Issue one",
                "state": "open",
                "labels": [{"name": "lane:a"}],
            },
        ],
        milestone_pages=[[{"number": 5, "title": "M5", "state": "open"}]],
    )

    generated_at_utc = "2026-02-23T20:10:00Z"
    run_a_issues = tmp_path / "a_issues.json"
    run_a_milestones = tmp_path / "a_milestones.json"
    run_b_issues = tmp_path / "b_issues.json"
    run_b_milestones = tmp_path / "b_milestones.json"

    code_a = capture_activation_snapshots.main(
        [
            "--issues-output",
            str(run_a_issues),
            "--milestones-output",
            str(run_a_milestones),
            "--generated-at-utc",
            generated_at_utc,
        ]
    )
    code_b = capture_activation_snapshots.main(
        [
            "--issues-output",
            str(run_b_issues),
            "--milestones-output",
            str(run_b_milestones),
            "--generated-at-utc",
            generated_at_utc,
        ]
    )

    assert code_a == 0
    assert code_b == 0
    assert run_a_issues.read_bytes() == run_b_issues.read_bytes()
    assert run_a_milestones.read_bytes() == run_b_milestones.read_bytes()
