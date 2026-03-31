#!/usr/bin/env python3
from __future__ import annotations

import json
import re
import subprocess
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
REPORT_DIR = ROOT / "tmp" / "reports" / "m317" / "M317-E001"
JSON_OUT = REPORT_DIR / "backlog_realignment_closeout_gate.json"
MD_OUT = REPORT_DIR / "backlog_realignment_closeout_gate.md"
REPO = "doublemover/Slopjective-C"
M317_TITLE = "M317 Backlog publication realignment and supersession hygiene"
EXPECTED_CODES = ["M317-A001", "M317-B001", "M317-B002", "M317-C001", "M317-D001", "M317-E001"]
CODE_RE = re.compile(r"^\[(M\d+)\]\[Lane-([A-E])\]\[([A-Z]\d{3})\] ")
EVIDENCE_FILES = [
    "tmp/reports/m317/M317-A001/backlog_overlap_supersession_inventory.json",
    "tmp/reports/m317/M317-B001/policy-summary.json",
    "tmp/reports/m317/M317-D001/backlog_consistency_audit.json",
    "tmp/github-publish/cleanup_acceleration_program/program_manifest.json",
    "tmp/github-publish/cleanup_acceleration_program/dependency_edges.json",
]


def run(command: list[str]) -> str:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr or completed.stdout or f"command failed: {' '.join(command)}")
    return completed.stdout


def run_json(command: list[str]) -> Any:
    return json.loads(run(command))


def read_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise TypeError(f"expected object in {path}")
    return payload


def flatten_pages(payload: list[Any]) -> list[dict[str, Any]]:
    result: list[dict[str, Any]] = []
    for page in payload:
        if isinstance(page, list):
            for item in page:
                if isinstance(item, dict):
                    result.append(item)
        elif isinstance(page, dict):
            result.append(page)
    return result


def parse_issue_code(title: str) -> str | None:
    match = CODE_RE.match(title)
    if not match:
        return None
    return f"{match.group(1)}-{match.group(3)}"


def main() -> int:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    mismatches: list[str] = []

    evidence_presence = {}
    for rel in EVIDENCE_FILES:
        evidence_presence[rel] = (ROOT / rel).exists()
        if not evidence_presence[rel]:
            mismatches.append(f"missing evidence file: {rel}")

    audit = read_json(ROOT / "tmp" / "reports" / "m317" / "M317-D001" / "backlog_consistency_audit.json")
    if not audit.get("passed"):
        mismatches.append("D001 backlog consistency audit did not pass")

    manifest = read_json(ROOT / "tmp" / "github-publish" / "cleanup_acceleration_program" / "program_manifest.json")
    m317_manifest = next((milestone for milestone in manifest.get("milestones", []) if milestone.get("code") == "M317"), None)
    if not m317_manifest:
        mismatches.append("M317 missing from cleanup program manifest")
    elif m317_manifest.get("publication_scope") != "internal-first":
        mismatches.append("M317 publication_scope is not internal-first in manifest")

    milestone_pages = run_json([
        "gh",
        "api",
        f"repos/{REPO}/milestones?state=open&per_page=100",
        "--paginate",
        "--slurp",
    ])
    milestones = flatten_pages(milestone_pages)
    live_m317 = next((milestone for milestone in milestones if milestone.get("title") == M317_TITLE), None)
    if not live_m317:
        mismatches.append("live GitHub milestone for M317 not found")
        milestone_number = None
        live_issues = []
    else:
        milestone_number = live_m317.get("number")
        live_issues = run_json([
            "gh",
            "api",
            f"repos/{REPO}/issues?milestone={milestone_number}&state=all&per_page=100",
            "--paginate",
            "--slurp",
        ])
        live_issues = flatten_pages(live_issues)

    live_codes = {}
    open_codes: list[str] = []
    closed_codes: list[str] = []
    for issue in live_issues:
        title = issue.get("title", "")
        if not isinstance(title, str):
            continue
        code = parse_issue_code(title)
        if not code:
            continue
        live_codes[code] = issue
        state = issue.get("state")
        if state == "open":
            open_codes.append(code)
        elif state == "closed":
            closed_codes.append(code)

    missing_live_codes = [code for code in EXPECTED_CODES if code not in live_codes]
    historical_extra_codes = sorted(code for code in live_codes if code not in EXPECTED_CODES)
    if missing_live_codes:
        mismatches.append(f"live M317 issue set is missing current manifest codes: {missing_live_codes}")
    if sorted(open_codes) != ["M317-E001"]:
        mismatches.append(f"live M317 open issue set mismatch: {sorted(open_codes)}")
    expected_closed_codes = sorted(code for code in EXPECTED_CODES if code != "M317-E001")
    missing_closed_codes = [code for code in expected_closed_codes if code not in closed_codes]
    if missing_closed_codes:
        mismatches.append(f"live M317 closed issue set is missing current closed codes: {missing_closed_codes}")

    commit_lines = [line for line in run(["git", "log", "--oneline", "--grep", "^M317-", "--max-count", "10"]).splitlines() if line]

    report = {
        "issue": "M317-E001",
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "repo": REPO,
        "live_m317_milestone_number": milestone_number,
        "expected_issue_codes": EXPECTED_CODES,
        "historical_extra_issue_codes": historical_extra_codes,
        "open_issue_codes": sorted(open_codes),
        "closed_issue_codes": sorted(closed_codes),
        "evidence_presence": evidence_presence,
        "consistency_audit_passed": bool(audit.get("passed")),
        "commit_lines": commit_lines,
        "mismatch_count": len(mismatches),
        "mismatches": mismatches,
        "passed": not mismatches,
    }

    JSON_OUT.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    lines = [
        "# M317-E001 Backlog Realignment Closeout Gate",
        "",
        f"- generated_at: `{report['generated_at']}`",
        f"- live_m317_milestone_number: `{milestone_number}`",
        f"- consistency_audit_passed: `{report['consistency_audit_passed']}`",
        f"- mismatch_count: `{len(mismatches)}`",
        f"- passed: `{report['passed']}`",
        "",
        "## Open issue codes",
    ]
    if open_codes:
        lines.extend(f"- `{code}`" for code in sorted(open_codes))
    else:
        lines.append("- none")
    lines.extend(["", "## Closed issue codes"])
    if closed_codes:
        lines.extend(f"- `{code}`" for code in sorted(closed_codes))
    else:
        lines.append("- none")
    lines.extend(["", "## Historical extra issue codes"])
    if historical_extra_codes:
        lines.extend(f"- `{code}`" for code in historical_extra_codes)
    else:
        lines.append("- none")
    lines.extend(["", "## Evidence presence"])
    for rel, exists in evidence_presence.items():
        lines.append(f"- `{rel}`: `{exists}`")
    lines.extend(["", "## Commits"])
    if commit_lines:
        lines.extend(f"- `{line}`" for line in commit_lines)
    else:
        lines.append("- none")
    lines.extend(["", "## Mismatches"])
    if mismatches:
        lines.extend(f"- {item}" for item in mismatches)
    else:
        lines.append("- none")
    MD_OUT.write_text("\n".join(lines) + "\n", encoding="utf-8")

    return 0 if not mismatches else 1


if __name__ == "__main__":
    raise SystemExit(main())
