import importlib.util
import io
import json
import sys
from contextlib import redirect_stderr, redirect_stdout
from pathlib import Path
from typing import Any

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "publish_objc3c_planning_issues.py"
SPEC = importlib.util.spec_from_file_location("publish_objc3c_planning_issues", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/publish_objc3c_planning_issues.py for tests.")
publish_objc3c_planning_issues = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = publish_objc3c_planning_issues
SPEC.loader.exec_module(publish_objc3c_planning_issues)

ROOT = Path(__file__).resolve().parents[2]


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = publish_objc3c_planning_issues.main(args)
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
                "labels": ["type:roadmap", "priority:P0", "area:governance"],
                "body": "<!-- draft-id: OC3-T01-001 -->\n## Objective\nFirst task.\n",
            },
            {
                "id": "OC3-T01-002",
                "gid": "T01",
                "index": 2,
                "title": "[OC3-T01-002] Second task",
                "milestoneTitle": "OC3-T01 Example",
                "labels": ["type:roadmap", "priority:P1", "area:runtime"],
                "body": "<!-- draft-id: OC3-T01-002 -->\n## Objective\nSecond task.\n",
            },
        ],
        "dependencies": [
            {
                "blocked": "OC3-T01-002",
                "blocker": "OC3-T01-001",
                "reason": "same-milestone sequencing",
            }
        ],
    }


def minimal_report() -> dict[str, Any]:
    return {
        "repository": "example/repo",
        "startedAtUtc": "2026-04-13T00:00:00Z",
        "updatedAtUtc": "2026-04-13T00:00:00Z",
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
        "dependencies": [],
        "failures": [],
    }


def test_dry_run_validates_payload_and_resolves_existing_dependency_mapping(tmp_path: Path) -> None:
    payload_path = tmp_path / "payload.json"
    report_path = tmp_path / "report.json"
    write_json(payload_path, minimal_payload())
    write_json(report_path, minimal_report())

    code, stdout, stderr = run_main(
        ["--payload", str(payload_path), "--report", str(report_path), "--write-report"]
    )

    assert code == 0
    assert stderr == ""
    assert "planning-issue-publisher: OK dry-run" in stdout
    assert "unresolved_dependencies=0" in stdout

    report = json.loads(report_path.read_text(encoding="utf-8"))
    assert report["contract_id"] == "objc3c.planning.github-publication-report.v1"
    assert report["milestoneCount"] == 1
    assert report["issueCount"] == 2
    assert report["dependencyCount"] == 1
    assert report["dependencies"][0]["blocked_issue_number"] == 8008
    assert report["dependencies"][0]["blocker_issue_number"] == 8007
    assert report["dependencies"][0]["native_relationship"] == "body-reference-and-label"


def test_fail_fast_when_payload_uses_repo_tmp_as_source_of_truth() -> None:
    tmp_source = ROOT / "tmp" / "planning_payload.json"

    try:
        publish_objc3c_planning_issues.assert_not_tmp_source(tmp_source)
    except Exception as exc:
        expected = (
            "refusing to use "
            "tmp/"
            " as planning source of "
            "truth"
        )
        assert expected in str(exc)
    else:
        raise AssertionError("tmp source path was accepted")


def test_fail_fast_when_issue_body_lacks_stable_draft_marker(tmp_path: Path) -> None:
    payload = minimal_payload()
    payload["issues"][0]["body"] = "## Objective\nFirst task.\n"
    payload_path = tmp_path / "payload.json"
    report_path = tmp_path / "report.json"
    write_json(payload_path, payload)
    write_json(report_path, minimal_report())

    code, stdout, stderr = run_main(["--payload", str(payload_path), "--report", str(report_path)])

    assert code == 1
    assert stdout == ""
    assert "body is missing its stable draft-id marker" in stderr


def test_fail_fast_when_dependency_references_unknown_issue(tmp_path: Path) -> None:
    payload = minimal_payload()
    payload["dependencies"][0]["blocker"] = "OC3-T99-999"
    payload_path = tmp_path / "payload.json"
    report_path = tmp_path / "report.json"
    write_json(payload_path, payload)
    write_json(report_path, minimal_report())

    code, stdout, stderr = run_main(["--payload", str(payload_path), "--report", str(report_path)])

    assert code == 1
    assert stdout == ""
    assert "dependency blocker id is unknown: OC3-T99-999" in stderr


def test_relationship_section_uses_github_numbers_when_available() -> None:
    payload = minimal_payload()
    _, blocks = publish_objc3c_planning_issues.dependencies_by_issue(payload)

    section = publish_objc3c_planning_issues.render_relationship_section(
        "OC3-T01-001",
        minimal_report()["issues"],
        {},
        blocks,
    )

    assert "## GitHub Relationships" in section
    assert "Blocks: #8008 (same-milestone sequencing)" in section
    assert "OC3-T01-002" not in section


def test_apply_relationship_section_replaces_only_managed_block() -> None:
    original = "\n".join(
        [
            "<!-- draft-id: OC3-T01-001 -->",
            "## Objective",
            "Keep this body.",
            "",
            "<!-- objc3c-publisher:relationships:start -->",
            "## GitHub Relationships",
            "- Blocks: #1",
            "<!-- objc3c-publisher:relationships:end -->",
            "",
            "## Notes",
            "Keep this too.",
        ]
    )
    replacement = "\n".join(
        [
            "<!-- objc3c-publisher:relationships:start -->",
            "## GitHub Relationships",
            "- Blocks: #8008",
            "<!-- objc3c-publisher:relationships:end -->",
        ]
    )

    updated = publish_objc3c_planning_issues.apply_relationship_section(original, replacement)

    assert "Keep this body." in updated
    assert "Keep this too." in updated
    assert "- Blocks: #1" not in updated
    assert "- Blocks: #8008" in updated
