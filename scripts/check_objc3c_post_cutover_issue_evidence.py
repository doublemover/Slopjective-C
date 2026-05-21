#!/usr/bin/env python3
"""Validate post-cutover Objective-C 3 issue evidence coverage."""

from __future__ import annotations

from dataclasses import dataclass
import json
import re
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.json_io import load_json_object, write_json_file
from objc3c_tooling.paths import repo_rel
from scripts.objc3c_workflow.public_command_api import public_workflow_action_names


ISSUE_EVIDENCE_PATH = ROOT / "docs/issues/objc3_next_8153_8179_evidence.md"
CAPABILITY_MATRIX_PATH = ROOT / "docs/support/capability_matrix.json"
EVIDENCE_MAP_PATH = ROOT / "docs/support/evidence_map.json"
SUMMARY_PATH = ROOT / "tmp/reports/post-cutover/post-cutover-issue-evidence-summary.json"
REQUIRED_ISSUES = tuple(f"#{number}" for number in range(8153, 8180))
GENERATED_OUTPUT_PREFIXES = ("tmp/", "checked_outputs/")
FORBIDDEN_SOURCE_PREFIXES = GENERATED_OUTPUT_PREFIXES
COMMAND_PREFIX = "npm run objc3c -- "


@dataclass(frozen=True)
class IssueEvidenceRow:
    issue: str
    local_evidence: str
    public_replay_surface: str
    boundary: str


def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def split_markdown_row(line: str) -> list[str]:
    return [cell.strip() for cell in line.strip().strip("|").split("|")]


def code_spans(text: str) -> list[str]:
    return re.findall(r"`([^`]+)`", text)


def parse_issue_rows(markdown: str) -> list[IssueEvidenceRow]:
    rows: list[IssueEvidenceRow] = []
    for raw_line in markdown.splitlines():
        line = raw_line.strip()
        if not line.startswith("| #"):
            continue
        cells = split_markdown_row(line)
        if len(cells) != 4:
            continue
        rows.append(
            IssueEvidenceRow(
                issue=cells[0],
                local_evidence=cells[1],
                public_replay_surface=cells[2],
                boundary=cells[3],
            )
        )
    return rows


def parse_owned_capabilities(markdown: str) -> list[str]:
    in_section = False
    capabilities: list[str] = []
    for raw_line in markdown.splitlines():
        line = raw_line.strip()
        if line == "## Capability Rows Owned By This Slice":
            in_section = True
            continue
        if in_section and line.startswith("## "):
            break
        if not in_section or not line.startswith("- "):
            continue
        spans = code_spans(line)
        if spans:
            capabilities.append(spans[0])
            continue
        capabilities.append(line.removeprefix("- ").strip())
    return capabilities


def action_name_from_command(command: str) -> str:
    if not command.startswith(COMMAND_PREFIX):
        return ""
    return command.removeprefix(COMMAND_PREFIX).split(" ", 1)[0]


def is_repo_source_path(value: str) -> bool:
    if value.startswith(COMMAND_PREFIX):
        return False
    if "://" in value:
        return False
    if value.startswith("#"):
        return False
    return "/" in value or value.endswith((".json", ".md", ".py", ".cpp", ".h", ".objc3"))


def validate_issue_rows(
    *,
    rows: list[IssueEvidenceRow],
    registered_actions: set[str],
    failures: list[str],
) -> dict[str, Any]:
    by_issue: dict[str, list[IssueEvidenceRow]] = {}
    for row in rows:
        by_issue.setdefault(row.issue, []).append(row)

    for issue in REQUIRED_ISSUES:
        matches = by_issue.get(issue, [])
        expect(len(matches) == 1, f"{issue}: expected exactly one evidence row, found {len(matches)}", failures)

    unexpected = sorted(issue for issue in by_issue if issue not in REQUIRED_ISSUES)
    for issue in unexpected:
        failures.append(f"{issue}: unexpected issue evidence row")

    source_paths: set[str] = set()
    public_actions: set[str] = set()
    for row in rows:
        if row.issue not in REQUIRED_ISSUES:
            continue
        expect(row.local_evidence != "", f"{row.issue}: local evidence is empty", failures)
        expect(row.public_replay_surface != "", f"{row.issue}: public replay surface is empty", failures)
        expect(row.boundary != "", f"{row.issue}: boundary is empty", failures)
        expect("TBD" not in row.boundary.upper(), f"{row.issue}: boundary still contains TBD", failures)
        expect("pending" not in row.boundary.lower(), f"{row.issue}: boundary still contains pending", failures)

        local_paths = [span for span in code_spans(row.local_evidence) if is_repo_source_path(span)]
        expect(bool(local_paths), f"{row.issue}: local evidence must name checked-in source paths", failures)
        for raw_path in local_paths:
            normalized = raw_path.replace("\\", "/")
            source_paths.add(normalized)
            if normalized.startswith(FORBIDDEN_SOURCE_PREFIXES):
                failures.append(f"{row.issue}: generated output cannot be source truth: {normalized}")
                continue
            if not (ROOT / normalized).exists():
                failures.append(f"{row.issue}: local evidence path does not exist: {normalized}")

        commands = [
            span
            for span in code_spans(row.public_replay_surface)
            if span.startswith(COMMAND_PREFIX)
        ]
        expect(bool(commands), f"{row.issue}: replay surface must name npm run objc3c command", failures)
        for command in commands:
            action = action_name_from_command(command)
            public_actions.add(action)
            if not action:
                failures.append(f"{row.issue}: malformed public command: {command}")
                continue
            if action not in registered_actions:
                failures.append(f"{row.issue}: public action is not registered: {action}")

    return {
        "issue_count": len(rows),
        "source_path_count": len(source_paths),
        "public_action_count": len(public_actions),
        "public_actions": sorted(public_actions),
    }


def capability_rows(matrix: dict[str, Any]) -> dict[str, dict[str, Any]]:
    rows = matrix.get("capabilities", [])
    if not isinstance(rows, list):
        return {}
    return {
        str(row.get("id")): row
        for row in rows
        if isinstance(row, dict) and isinstance(row.get("id"), str)
    }


def evidence_row_keys(evidence_map: dict[str, Any]) -> set[tuple[str, str]]:
    rows = evidence_map.get("rows", [])
    if not isinstance(rows, list):
        return set()
    keys: set[tuple[str, str]] = set()
    for row in rows:
        if not isinstance(row, dict):
            continue
        capability_id = row.get("capability_id")
        support_claim = row.get("support_claim")
        if isinstance(capability_id, str) and isinstance(support_claim, str):
            keys.add((capability_id, support_claim))
    return keys


def validate_owned_capabilities(
    *,
    owned_capabilities: list[str],
    matrix: dict[str, Any],
    evidence_map: dict[str, Any],
    failures: list[str],
) -> dict[str, Any]:
    expect(bool(owned_capabilities), "owned capability section must not be empty", failures)
    duplicates = sorted(
        capability
        for capability in set(owned_capabilities)
        if owned_capabilities.count(capability) > 1
    )
    for capability in duplicates:
        failures.append(f"owned capability listed more than once: {capability}")

    rows_by_id = capability_rows(matrix)
    evidence_keys = evidence_row_keys(evidence_map)
    implemented_count = 0
    support_claim_count = 0

    for capability_id in owned_capabilities:
        row = rows_by_id.get(capability_id)
        if row is None:
            failures.append(f"owned capability is not present in capability matrix: {capability_id}")
            continue
        state = row.get("state")
        support_claims = row.get("support_claims", [])
        if state == "implemented":
            implemented_count += 1
            if not isinstance(support_claims, list) or not support_claims:
                failures.append(f"{capability_id}: implemented capability has no support claims")
                continue
            for claim in support_claims:
                if not isinstance(claim, str):
                    failures.append(f"{capability_id}: support claim must be a string")
                    continue
                support_claim_count += 1
                if (capability_id, claim) not in evidence_keys:
                    failures.append(
                        f"{capability_id}: support claim lacks evidence-map row: {claim}"
                    )

    return {
        "owned_capability_count": len(owned_capabilities),
        "implemented_owned_capability_count": implemented_count,
        "support_claim_count": support_claim_count,
    }


def validate_markdown_text(
    markdown: str,
    *,
    matrix: dict[str, Any],
    evidence_map: dict[str, Any],
    registered_actions: set[str],
) -> dict[str, Any]:
    failures: list[str] = []
    rows = parse_issue_rows(markdown)
    owned_capabilities = parse_owned_capabilities(markdown)
    issue_summary = validate_issue_rows(
        rows=rows,
        registered_actions=registered_actions,
        failures=failures,
    )
    capability_summary = validate_owned_capabilities(
        owned_capabilities=owned_capabilities,
        matrix=matrix,
        evidence_map=evidence_map,
        failures=failures,
    )
    return {
        "ok": not failures,
        "issue_evidence": {
            "path": repo_rel(ISSUE_EVIDENCE_PATH),
            **issue_summary,
        },
        "capability_rows": capability_summary,
        "failures": failures,
    }


def validate_current_state() -> dict[str, Any]:
    markdown = ISSUE_EVIDENCE_PATH.read_text(encoding="utf-8")
    matrix = load_json_object(CAPABILITY_MATRIX_PATH)
    evidence_map = load_json_object(EVIDENCE_MAP_PATH)
    registered_actions = set(public_workflow_action_names())
    return validate_markdown_text(
        markdown,
        matrix=matrix,
        evidence_map=evidence_map,
        registered_actions=registered_actions,
    )


def main() -> int:
    summary = validate_current_state()
    write_json_file(SUMMARY_PATH, summary)
    print(json.dumps(summary, indent=2, sort_keys=True))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
