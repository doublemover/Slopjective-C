import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path
from typing import Any

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "audit_objc3c_planning_publication.py"
SPEC = importlib.util.spec_from_file_location("audit_objc3c_planning_publication", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/audit_objc3c_planning_publication.py for tests.")
audit_objc3c_planning_publication = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = audit_objc3c_planning_publication
SPEC.loader.exec_module(audit_objc3c_planning_publication)


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = audit_objc3c_planning_publication.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def write_json(path: Path, payload: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def minimal_payload() -> dict[str, Any]:
    return {
        "repository": "example/repo",
        "generatedAtUtc": "2026-04-13T00:00:00Z",
        "rules": {
            "acceptGithubAssignedNumbers": True,
            "doNotUseTmpAsSourceOfTruth": True,
            "createSubIssuesAsChecklistItemsOnly": True,
            "publishNativeBlockedByRelationships": True,
        },
        "milestones": [
            {
                "id": "OC3-T01",
                "gid": "T01",
                "order": 1,
                "title": "OC3-T01 Example",
                "description": "Example milestone.",
            }
        ],
        "issues": [
            {
                "id": "OC3-T01-001",
                "gid": "T01",
                "index": 1,
                "title": "[OC3-T01-001] First task",
                "milestoneTitle": "OC3-T01 Example",
                "labels": ["type:roadmap", "priority:P0"],
                "body": "<!-- draft-id: OC3-T01-001 -->\n## Objective\nFirst task.\n",
            },
            {
                "id": "OC3-T01-002",
                "gid": "T01",
                "index": 2,
                "title": "[OC3-T01-002] Second task",
                "milestoneTitle": "OC3-T01 Example",
                "labels": ["type:roadmap", "priority:P1"],
                "body": "<!-- draft-id: OC3-T01-002 -->\n## Objective\nSecond task.\n",
            },
        ],
        "dependencies": [
            {"blocked": "OC3-T01-002", "blocker": "OC3-T01-001", "reason": "sequence"}
        ],
    }


def minimal_report() -> dict[str, Any]:
    return {
        "contract_id": "objc3c.planning.github-publication-report.v1",
        "repository": "example/repo",
        "startedAtUtc": "2026-04-13T00:00:00Z",
        "updatedAtUtc": "2026-04-13T01:00:00Z",
        "milestoneCount": 1,
        "issueCount": 2,
        "dependencyCount": 1,
        "milestones": {
            "OC3-T01": {
                "number": 414,
                "id": 1,
                "title": "OC3-T01 Example",
                "html_url": "https://example.test/milestone/414",
                "reused": False,
            }
        },
        "issues": {
            "OC3-T01-001": {
                "number": 8007,
                "id": 10,
                "node_id": "I_1",
                "title": "[OC3-T01-001] First task",
                "html_url": "https://example.test/issues/8007",
                "milestone_number": 414,
                "reused": False,
            },
            "OC3-T01-002": {
                "number": 8008,
                "id": 11,
                "node_id": "I_2",
                "title": "[OC3-T01-002] Second task",
                "html_url": "https://example.test/issues/8008",
                "milestone_number": 414,
                "reused": False,
            },
        },
        "dependencies": [
            {
                "blocked": "OC3-T01-002",
                "blocked_number": 8008,
                "blocker": "OC3-T01-001",
                "blocker_number": 8007,
                "reason": "sequence",
                "status": "resolved",
                "blocked_issue_number": 8008,
                "blocker_issue_number": 8007,
                "resolved": True,
                "native_relationship": "body-reference-and-label",
            }
        ],
        "failures": [],
    }


def test_update_references_and_write_drift_report(tmp_path: Path) -> None:
    payload_path = tmp_path / "payload.json"
    report_path = tmp_path / "publication.json"
    markdown_path = tmp_path / "publication.md"
    drift_path = tmp_path / "drift.json"
    write_json(payload_path, minimal_payload())
    write_json(report_path, minimal_report())

    code, stdout, stderr = run_main(
        [
            "--payload",
            str(payload_path),
            "--publication-report",
            str(report_path),
            "--markdown-report",
            str(markdown_path),
            "--output",
            str(drift_path),
            "--update-payload-published",
            "--update-markdown-report",
            "--write",
        ]
    )

    assert code == 0
    assert stderr == ""
    assert "planning-publication-drift-audit: OK" in stdout

    payload = json.loads(payload_path.read_text(encoding="utf-8"))
    report = json.loads(report_path.read_text(encoding="utf-8"))
    drift = json.loads(drift_path.read_text(encoding="utf-8"))
    assert payload["published"] == audit_objc3c_planning_publication.publication_snapshot(report)
    assert "#8008" in markdown_path.read_text(encoding="utf-8")
    assert drift["contract_id"] == "objc3c.planning.github-publication-drift-report.v1"
    assert drift["failure_count"] == 0


def test_check_fails_when_markdown_report_is_stale(tmp_path: Path) -> None:
    payload = minimal_payload()
    report = minimal_report()
    payload["published"] = audit_objc3c_planning_publication.publication_snapshot(report)
    payload_path = tmp_path / "payload.json"
    report_path = tmp_path / "publication.json"
    markdown_path = tmp_path / "publication.md"
    drift_path = tmp_path / "drift.json"
    write_json(payload_path, payload)
    write_json(report_path, report)
    markdown_path.write_text("stale\n", encoding="utf-8")
    write_json(drift_path, {})

    code, stdout, stderr = run_main(
        [
            "--payload",
            str(payload_path),
            "--publication-report",
            str(report_path),
            "--markdown-report",
            str(markdown_path),
            "--output",
            str(drift_path),
            "--check",
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "markdown-report-drift" in stderr


def test_check_fails_when_dependency_number_drifts(tmp_path: Path) -> None:
    payload = minimal_payload()
    report = minimal_report()
    payload["published"] = audit_objc3c_planning_publication.publication_snapshot(report)
    report["dependencies"][0]["blocked_number"] = 9999
    payload_path = tmp_path / "payload.json"
    report_path = tmp_path / "publication.json"
    markdown_path = tmp_path / "publication.md"
    drift_path = tmp_path / "drift.json"
    write_json(payload_path, payload)
    write_json(report_path, report)
    markdown_path.write_text(audit_objc3c_planning_publication.render_markdown_report(report), encoding="utf-8")

    code, stdout, stderr = run_main(
        [
            "--payload",
            str(payload_path),
            "--publication-report",
            str(report_path),
            "--markdown-report",
            str(markdown_path),
            "--output",
            str(drift_path),
        ]
    )

    assert code == 1
    assert stdout == ""
    assert "dependency-number-drift" in stderr
