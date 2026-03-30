#!/usr/bin/env python3
"""Publish the live public conformance report artifacts and summary."""

from __future__ import annotations

import json
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
SOURCE_CHECK = ROOT / "scripts" / "check_public_conformance_reporting_source_surface.py"
SCHEMA_CHECK = ROOT / "scripts" / "check_public_conformance_schema_surface.py"
SCORECARD_BUILD = ROOT / "scripts" / "build_objc3c_public_conformance_scorecard.py"
SOURCE_SUMMARY = ROOT / "tmp" / "reports" / "public-conformance" / "source-surface-summary.json"
SCHEMA_SUMMARY = ROOT / "tmp" / "reports" / "public-conformance" / "schema-surface-summary.json"
SCORECARD_SUMMARY = ROOT / "tmp" / "reports" / "public-conformance" / "scorecard-summary.json"
PUBLIC_SUMMARY = ROOT / "tmp" / "reports" / "public-conformance" / "public-summary.json"
ARTIFACT_ROOT = ROOT / "tmp" / "artifacts" / "public-conformance"
PUBLISHED_SCORECARD = ARTIFACT_ROOT / "scorecard" / "public-conformance-scorecard.json"
PUBLISHED_BADGE = ARTIFACT_ROOT / "badge" / "public-conformance-badge.json"
PUBLISHED_REPORT_MD = ARTIFACT_ROOT / "report" / "public-conformance-report.md"
SUMMARY_CONTRACT_ID = "objc3c.public_conformance_reporting.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_capture(command: Sequence[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def ensure_success(path: Path, script: Path) -> dict[str, Any]:
    if not path.is_file():
        result = run_capture([sys.executable, str(script)])
        if result.returncode != 0:
            raise RuntimeError(f"failed to build required report via {repo_rel(script)}")
    payload = load_json(path)
    if payload.get("status") != "PASS":
        raise RuntimeError(f"required report did not pass: {repo_rel(path)}")
    return payload


def pluralize(value: int, singular: str, plural: str | None = None) -> str:
    if value == 1:
        return singular
    return plural or f"{singular}s"


def main() -> int:
    try:
        source_summary = ensure_success(SOURCE_SUMMARY, SOURCE_CHECK)
        schema_summary = ensure_success(SCHEMA_SUMMARY, SCHEMA_CHECK)
        scorecard = ensure_success(SCORECARD_SUMMARY, SCORECARD_BUILD)
    except RuntimeError as exc:
        print(f"objc3c-public-conformance-report: FAIL\n- {exc}", file=sys.stderr)
        return 1

    badge = str(scorecard["badge"])
    score = int(scorecard["score"])
    public_status = str(scorecard["public_status"])
    claimability = scorecard["claimability_counts"]
    deductions = scorecard.get("deductions", [])
    hard_blocks = scorecard.get("hard_blocks", [])

    if public_status == "pass":
        headline = f"Objective-C 3 public conformance is claim-ready with a stability score of {score}."
    elif public_status == "caution":
        headline = f"Objective-C 3 public conformance is publishable with caution at a stability score of {score}."
    else:
        headline = f"Objective-C 3 public conformance is blocked at a stability score of {score}."

    report_lines = [
        (
            "Corpus integration is "
            f"{scorecard['upstream_status']['corpus_integration']} with "
            f"{claimability['retained_suite_count']} retained suites and "
            f"{claimability['manifest_summary_count']} manifest summaries."
        ),
        (
            "External validation is "
            f"{scorecard['upstream_status']['external_validation_integration']} with "
            f"{claimability['accepted_fixture_count']} accepted {pluralize(claimability['accepted_fixture_count'], 'fixture')}, "
            f"{claimability['redacted_fixture_count']} redacted {pluralize(claimability['redacted_fixture_count'], 'fixture')}, and "
            f"{claimability['blocked_fixture_count']} blocked {pluralize(claimability['blocked_fixture_count'], 'fixture')}."
        ),
        (
            "Checked-in dashboard and release-evidence schema anchors are "
            + ("present." if not scorecard["missing_schema_anchors"] else "missing.")
        ),
    ]
    if deductions:
        report_lines.append(
            "Applied deductions: "
            + "; ".join(f"{entry['deduction_id']} (-{entry['amount']})" for entry in deductions)
            + "."
        )
    if hard_blocks:
        report_lines.append("Hard blocks: " + "; ".join(str(entry) for entry in hard_blocks) + ".")

    evidence_paths = [
        repo_rel(SOURCE_SUMMARY),
        repo_rel(SCHEMA_SUMMARY),
        repo_rel(SCORECARD_SUMMARY),
        scorecard["upstream_reports"]["corpus_integration"],
        scorecard["upstream_reports"]["external_validation_integration"],
        scorecard["upstream_reports"]["external_validation_publication"],
    ]

    PUBLISHED_SCORECARD.parent.mkdir(parents=True, exist_ok=True)
    PUBLISHED_BADGE.parent.mkdir(parents=True, exist_ok=True)
    PUBLISHED_REPORT_MD.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(SCORECARD_SUMMARY, PUBLISHED_SCORECARD)
    PUBLISHED_BADGE.write_text(
        json.dumps(
            {
                "badge": badge,
                "public_status": public_status,
                "score": score,
                "claim_ready": bool(scorecard["claim_ready"]),
            },
            indent=2,
        )
        + "\n",
        encoding="utf-8",
    )
    markdown_lines = [
        "# Objective-C 3 Public Conformance Report",
        "",
        f"- Public status: `{public_status}`",
        f"- Badge: `{badge}`",
        f"- Stability score: `{score}`",
        f"- Claim ready: `{str(bool(scorecard['claim_ready'])).lower()}`",
        "",
        headline,
        "",
        "## Summary",
        "",
    ]
    markdown_lines.extend(f"- {line}" for line in report_lines)
    markdown_lines.extend(
        [
            "",
            "## Evidence",
            "",
        ]
    )
    markdown_lines.extend(f"- `{path}`" for path in evidence_paths)
    PUBLISHED_REPORT_MD.write_text("\n".join(markdown_lines) + "\n", encoding="utf-8")

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "source_surface_summary_path": repo_rel(SOURCE_SUMMARY),
        "schema_surface_summary_path": repo_rel(SCHEMA_SUMMARY),
        "scorecard_summary_path": repo_rel(SCORECARD_SUMMARY),
        "report_markdown_path": repo_rel(PUBLISHED_REPORT_MD),
        "public_status": public_status,
        "badge": badge,
        "score": score,
        "headline": headline,
        "report_lines": report_lines,
        "evidence_paths": evidence_paths,
    }
    PUBLIC_SUMMARY.parent.mkdir(parents=True, exist_ok=True)
    PUBLIC_SUMMARY.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(PUBLIC_SUMMARY)}")
    print(f"published_scorecard: {repo_rel(PUBLISHED_SCORECARD)}")
    print(f"published_badge: {repo_rel(PUBLISHED_BADGE)}")
    print(f"published_report_markdown: {repo_rel(PUBLISHED_REPORT_MD)}")
    print("objc3c-public-conformance-report: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
